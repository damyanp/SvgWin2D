using System.Numerics;
using Microsoft.Graphics.Canvas;
using Microsoft.Graphics.Canvas.UI.Xaml;
using SvgWin2D;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Threading.Tasks;
using Windows.Data.Xml.Dom;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.Storage;
using Windows.Storage.Pickers;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;
using Windows.UI;
using Microsoft.Graphics.Canvas.Text;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

namespace SvgViewer
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class MainPage : Page
    {        
        SvgDrawing svgDrawing;
        IStorageFile loadedSvgFile;
        IStorageFile wantedSvgFile;

        public MainPage()
        {
            this.InitializeComponent();
        }

        private async void OnLoad(object sender, RoutedEventArgs e)
        {
            var picker = new FileOpenPicker();
            picker.SuggestedStartLocation = PickerLocationId.PicturesLibrary;
            picker.FileTypeFilter.Add(".svg");

            var pickedFile = await picker.PickSingleFileAsync();
            if (pickedFile != null)
            {
                wantedSvgFile = pickedFile;
                svgDrawing = null;
                canvas.Invalidate();
            }
        }

        Task loadDrawingTask;

        void EnsureSvgDrawingLoaded(ICanvasResourceCreator resourceCreator)
        {
            if (IsLoadInProgress())
                return;

            if (wantedSvgFile != loadedSvgFile)
            {
                loadDrawingTask = LoadDrawing(resourceCreator);
            }
        }

        async Task LoadDrawing(ICanvasResourceCreator resourceCreator)
        {
            var svgDocument = await XmlDocument.LoadFromFileAsync(wantedSvgFile, new XmlLoadSettings() { ProhibitDtd = false });
            svgDrawing = await SvgDrawing.LoadAsync(resourceCreator, svgDocument);
            loadedSvgFile = wantedSvgFile;
            canvas.Invalidate();
        }

        bool IsLoadInProgress()
        { 
            if (loadDrawingTask == null)
                return false;

            if (!loadDrawingTask.IsCompleted)
                return true;

            try
            {
                loadDrawingTask.Wait();
            }
            catch (AggregateException aggregateException)
            {
                aggregateException.Handle(exception => { throw exception; });
            }
            finally
            {
                loadDrawingTask = null;
            }

            return false;     
        }

        private void OnDraw(CanvasControl sender, CanvasDrawEventArgs args)
        {
            var ds = args.DrawingSession;

            EnsureSvgDrawingLoaded(sender);

            if (svgDrawing == null)
            {
                if (wantedSvgFile != null)
                {
                    ds.DrawText("Loading...",
                        sender.Size.ToVector2() / 2,
                        Colors.Black,
                        new CanvasTextFormat()
                        {
                            HorizontalAlignment = CanvasHorizontalAlignment.Center,
                            VerticalAlignment = CanvasVerticalAlignment.Center
                        });
                }
            }
            else
            {
                var svgImage = svgDrawing.Draw(sender.Size);

                var bounds = svgImage.GetBounds(ds);

                if (bounds.Width > sender.Size.Width || bounds.Height > sender.Size.Height)
                {
                    double scale = Math.Min(sender.Size.Height / bounds.Height, sender.Size.Width / bounds.Width);
                    Size destSize = new Size(bounds.Width * scale, bounds.Height * scale);

                    args.DrawingSession.DrawImage(
                        svgImage,
                        new Rect(new Point(), destSize),
                        bounds);
                }
                else
                {
                    args.DrawingSession.DrawImage(
                        svgImage,
                        -(float)bounds.Left, -(float)bounds.Top);
                }
            }
        }
    }
}
