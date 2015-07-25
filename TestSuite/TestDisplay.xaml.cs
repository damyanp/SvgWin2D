using Microsoft.Graphics.Canvas;
using Microsoft.Graphics.Canvas.Text;
using Microsoft.Graphics.Canvas.UI.Xaml;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Threading.Tasks;
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

// The User Control item template is documented at http://go.microsoft.com/fwlink/?LinkId=234236

namespace TestSuite
{
    class TestDisplayData
    {
        SvgTest test;

        public CanvasBitmap ReferencePng { get; private set; }

        public static async Task<TestDisplayData> CreateAsync(SvgTest test)
        {
            var data = new TestDisplayData(test);

            var device = CanvasDevice.GetSharedDevice(false);

            data.ReferencePng = await CanvasBitmap.LoadAsync(device, new Uri(test.ReferencePngUri));

            return data;
        }

        TestDisplayData(SvgTest test)
        {
            this.test = test;
        }

        public string Name { get { return test.Name; } }
        public string Description { get { return "description"; } }
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
            ds.DrawImage(data.ReferencePng);

            var bounds = data.ReferencePng.Bounds;
            ds.DrawText("Reference Image", 0, (float)bounds.Bottom, Colors.Black, new CanvasTextFormat() { VerticalAlignment = CanvasVerticalAlignment.Top });
        }
    }
}