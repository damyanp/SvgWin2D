using System;
using System.IO;
using System.Threading.Tasks;
using Windows.Storage;
using Windows.Storage.Streams;
using Windows.Web.Http;

namespace TestSuite
{
    static class CachedData
    {
        public static async Task<IRandomAccessStream> GetRandomAccessStreamAsync(Uri uri)
        {
            var file = await GetStorageFileAsync(uri);
            return await file.OpenAsync(FileAccessMode.Read);
        }

        public static async Task<IStorageFile> GetStorageFileAsync(Uri uri)
        {
            var filename = Path.GetFileName(uri.GetComponents(UriComponents.Path, UriFormat.SafeUnescaped));

            var tempFolder = ApplicationData.Current.TemporaryFolder;
            
            var file = await tempFolder.TryGetItemAsync(filename) as IStorageFile;

            if (file != null)
                return file;

            file = await tempFolder.CreateFileAsync(filename);

            var buffer = await new HttpClient().GetBufferAsync(uri);

            await Windows.Storage.FileIO.WriteBufferAsync(file, buffer);

            return file;
        }

    }
}
