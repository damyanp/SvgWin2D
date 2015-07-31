using Microsoft.Graphics.Canvas;
using Microsoft.Graphics.Canvas.Effects;
using Microsoft.Graphics.Canvas.UI.Xaml;
using SvgWin2D;
using System;
using System.IO;
using System.Numerics;
using System.Threading.Tasks;
using Windows.Data.Xml.Dom;
using Windows.Foundation;
using Windows.UI;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;


namespace TestSuite
{
    class TestDisplayData
    {
        SvgTest test;

        public CanvasBitmap ReferencePng { get; private set; }
        public SvgDrawing Drawing { get; private set; }
        public ICanvasImage SvgImage { get; private set; }

        public static async Task<TestDisplayData> CreateAsync(SvgTest test)
        {
            var data = new TestDisplayData(test);

            var device = CanvasDevice.GetSharedDevice(false);

            data.ReferencePng = await DownloadPng(device, new Uri(test.ReferencePngUri));

            var svgFile = await CachedData.GetStorageFileAsync(new Uri(test.SvgUri));
            var svgDocument = await XmlDocument.LoadFromFileAsync(svgFile, new XmlLoadSettings() { ProhibitDtd = false });
            data.Drawing = await SvgDrawing.LoadAsync(device, svgDocument);
            data.SvgImage = data.Drawing.Draw(new Size(480, 360));

            var description = svgDocument.SelectSingleNodeNS("//d:testDescription", "xmlns:d='http://www.w3.org/2000/02/svg/testsuite/description/'");
            if (description != null)
                data.Description = description.InnerText;

            return data;
        }

        private static async Task<CanvasBitmap> DownloadPng(CanvasDevice device, Uri uri)
        {
            try
            {
                return await CanvasBitmap.LoadAsync(device, await CachedData.GetRandomAccessStreamAsync(uri));
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
            instance.Invalidate();
        }

        public TestDisplay()
        {
            DataContext = null;
            this.InitializeComponent();           
        }

        void Invalidate()
        {
            svgCanvas.Invalidate();
            pngCanvas.Invalidate();
            diffCanvas.Invalidate();
        }

        void SvgCanvas_Draw(CanvasControl sender, CanvasDrawEventArgs args)
        {
            var ds = args.DrawingSession;

            var data = DataContext as TestDisplayData;
            if (data == null)
                return;

            ds.DrawImage(CreateCheckerBoard(sender), new Rect(0, 0, 480, 360), new Rect(0, 0, 480, 360));

            ds.DrawImage(data.SvgImage, new Rect(0, 0, 480, 360), new Rect(0, 0, 480, 360));
        }

        void PngCanvas_Draw(CanvasControl sender, CanvasDrawEventArgs args)
        {
            var ds = args.DrawingSession;

            var data = DataContext as TestDisplayData;
            if (data == null)
                return;

            ds.DrawImage(CreateCheckerBoard(sender), new Rect(0, 0, 480, 360), new Rect(0, 0, 480, 360));
            ds.DrawImage(data.ReferencePng, new Rect(0, 0, 480, 360), new Rect(0, 0, 480, 360));
        }

        void DiffCanvas_Draw(CanvasControl sender, CanvasDrawEventArgs args)
        {
            var ds = args.DrawingSession;

            var data = DataContext as TestDisplayData;
            if (data == null)
                return;

            var white = new ColorSourceEffect() { Color = Colors.White };

            var refImg = new CompositeEffect() { Sources = { white, data.ReferencePng } };
            var svgImg = new CompositeEffect() { Sources = { white, data.SvgImage } };

            var blendA = new BlendEffect() { Background = data.SvgImage, Foreground = data.ReferencePng, Mode = BlendEffectMode.Difference };
            var blendB = new BlendEffect() { Background = data.SvgImage, Foreground = data.ReferencePng, Mode = BlendEffectMode.Difference };

            ICanvasImage display = new BlendEffect()
            {
                Background = blendA,
                Foreground = blendB,
                Mode = BlendEffectMode.Overlay
            };

            display = blendA;
            /*
            display = new MorphologyEffect()
            {
                Mode = MorphologyEffectMode.Erode,
                Width = 4,
                Height = 4,
                Source = display
            };

            display = new MorphologyEffect()
            {
                Mode = MorphologyEffectMode.Dilate,
                Width = 4,
                Height = 4,
                Source = display
            };*/

            display = new InvertEffect()
            {
                Source = display
            };

            ds.DrawImage(display, new Rect(0, 0, 480, 360), new Rect(0, 0, 480, 360));

        }


        ICanvasImage CreateCheckerBoard(CanvasControl control)
        {
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
                        Source = CanvasBitmap.CreateFromColors(control, twoByTwoChecker, 2, 2),
                        ExtendX = CanvasEdgeBehavior.Wrap,
                        ExtendY = CanvasEdgeBehavior.Wrap
                    },
                    Scale = new Vector2(8, 8),
                    InterpolationMode = CanvasImageInterpolation.NearestNeighbor
                }
            };

            return checker;
        }
    }
}