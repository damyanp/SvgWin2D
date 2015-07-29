using Microsoft.Graphics.Canvas;
using Microsoft.Graphics.Canvas.Effects;
using Microsoft.Graphics.Canvas.Text;
using Microsoft.Graphics.Canvas.UI.Xaml;
using SvgWin2D;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Numerics;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Threading.Tasks;
using Windows.Data.Xml.Dom;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;
using Windows.Graphics.Effects;


// The User Control item template is documented at http://go.microsoft.com/fwlink/?LinkId=234236

namespace TestSuite
{
    class TestDisplayData
    {
        SvgTest test;

        public CanvasBitmap ReferencePng { get; private set; }
        public SvgDrawing Drawing { get; private set; }

        public static async Task<TestDisplayData> CreateAsync(SvgTest test)
        {
            var data = new TestDisplayData(test);

            var device = CanvasDevice.GetSharedDevice(false);

            data.ReferencePng = await DownloadPng(device, new Uri(test.ReferencePngUri));

            var svgDocument = await XmlDocument.LoadFromUriAsync(new Uri(test.SvgUri), new XmlLoadSettings() { ProhibitDtd = false });
            data.Drawing = await SvgDrawing.LoadAsync(device, svgDocument);

            var description = svgDocument.SelectSingleNodeNS("//d:testDescription", "xmlns:d='http://www.w3.org/2000/02/svg/testsuite/description/'");
            if (description != null)
                data.Description = description.InnerText;

            return data;
        }

        private static async Task<CanvasBitmap> DownloadPng(CanvasDevice device, Uri uri)
        {
            try
            {
                return await CanvasBitmap.LoadAsync(device, uri);
            }
            catch (FileNotFoundException)
            {
                var rt = new CanvasRenderTarget(device, 480, 360, 96);

                using (var ds = rt.CreateDrawingSession())
                {
                    ds.Clear(Colors.Transparent);
                    ds.DrawLine(0, 0, (float)rt.Size.Width, (float)rt.Size.Height, Colors.Black, 1);
                    ds.DrawLine(0, (float)rt.Size.Height, (float)rt.Size.Width, 0, Colors.Black, 1);
                }
                return rt;
            }
        }

        TestDisplayData(SvgTest test)
        {
            this.test = test;
        }

        public string Name { get { return test.Name; } }
        public string Description { get; set; }
    
    }

    public class TestDisplayDesignData
    {
        public string Name { get { return "any-test.svg"; } }
    }


    public sealed partial class TestDisplay : UserControl
    {
        public SvgTest SvgTest
        {
            get { return (SvgTest)GetValue(SvgTestProperty); }
            set { SetValue(SvgTestProperty, value); }
        }

        public static readonly DependencyProperty SvgTestProperty =
            DependencyProperty.Register(
                "SvgTest",
                typeof(SvgTest),
                typeof(TestDisplay),
                new PropertyMetadata(null, new PropertyChangedCallback(OnSvgTestChanged)));

        private static async void OnSvgTestChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
            var instance = d as TestDisplay;
            if (instance == null)
                return;

            instance.DataContext = await TestDisplayData.CreateAsync((SvgTest)e.NewValue);
            instance.canvas.Invalidate();
        }

        public TestDisplay()
        {
            DataContext = null;
            this.InitializeComponent();           
        }

        void CanvasControl_Draw(CanvasControl sender, CanvasDrawEventArgs args)
        {
            var data = DataContext as TestDisplayData;
            if (data == null)
                return;

            var ds = args.DrawingSession;

            var color1 = Color.FromArgb(255, 254, 254, 254);
            var color2 = Color.FromArgb(255, 250, 250, 250);

            Color[] twoByTwoChecker =
            {
                color1, color2,
                color2, color1
            };

            var checker = new DpiCompensationEffect
            {
                Source = new ScaleEffect
                {
                    Source = new BorderEffect
                    {
                        Source = CanvasBitmap.CreateFromColors(canvas, twoByTwoChecker, 2, 2),
                        ExtendX = CanvasEdgeBehavior.Wrap,
                        ExtendY = CanvasEdgeBehavior.Wrap
                    },
                    Scale = new Vector2(8, 8),
                    InterpolationMode = CanvasImageInterpolation.NearestNeighbor
                }
            };

            var svgImage = data.Drawing.Draw(new Size(480, 360));

            ds.DrawImage(checker, new Rect(0, 0, 480, 360), new Rect(0, 0, 480, 360));
            ds.DrawImage(svgImage, new Rect(0, 0, 480, 360), new Rect(0, 0, 480, 360));

            ds.DrawImage(checker, new Rect(500, 0, 480, 360), new Rect(0, 0, 480, 360));
            ds.DrawImage(data.ReferencePng, new Rect(500, 0, 480, 360), new Rect(0, 0, 480, 360));
        }
    }
}