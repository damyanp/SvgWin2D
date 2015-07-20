using Microsoft.Graphics.Canvas;
using Microsoft.Graphics.Canvas.UI;
using Microsoft.Graphics.Canvas.UI.Xaml;
using SvgWin2D;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Threading.Tasks;
using Windows.ApplicationModel;
using Windows.Data.Xml.Dom;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;
using System.Numerics;
using Windows.UI;

namespace Demo
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class MainPage : Page
    {
        public MainPage()
        {
            this.InitializeComponent();
        }

        CanvasCommandList svgCommandList;

        private void OnCreateResources(CanvasControl sender, CanvasCreateResourcesEventArgs args)
        {
            args.TrackAsyncAction(LoadAndDrawSvg(sender).AsAsyncAction());
        }

        async Task LoadAndDrawSvg(ICanvasResourceCreator resourceCreator)
        {
            var file = await Package.Current.InstalledLocation.GetFileAsync("FirstSVGTest.svg");
            var document = await XmlDocument.LoadFromFileAsync(file);

            var svgElement = document.GetElementsByTagName("svg").First();

            svgCommandList = SvgRenderer.Render(resourceCreator, svgElement);
        }

        private void OnDraw(CanvasControl sender, CanvasDrawEventArgs args)
        {
            var ds = args.DrawingSession;

            var bounds = svgCommandList.GetBounds(ds);

            var destCenter = sender.Size.ToVector2() / 2;

            var imgOffset = new Vector2((float)bounds.Left, (float)bounds.Top);
            var imgSize = new Vector2((float)bounds.Width, (float)bounds.Height);
            var destPos = destCenter - imgSize / 2 - imgOffset;

            ds.DrawImage(svgCommandList, destPos);
        }
    }
}
