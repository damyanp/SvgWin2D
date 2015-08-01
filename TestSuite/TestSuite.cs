using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.Data.Xml.Dom;
using Windows.Web.Http;
using Windows.Web.Http.Filters;

namespace TestSuite
{
    public class SvgTest
    {
        public string Name { get; private set; }
        public string Chapter
        {
            get
            {
                return Name.Substring(0, Name.IndexOf('-'));
            }
        }


        public SvgTest(string name)
        {
            Name = name;
        }

        public string ReferencePngUri
        {
            get
            {
                return "http://www.w3.org/Graphics/SVG/Test/20110816/png/" + Name.Replace(".svg", ".png");
            }
        }

        public string SvgUri
        {
            get
            {
                return "http://www.w3.org/Graphics/SVG/Test/20110816/svg/" + Name;
            }
        }

    }


    public class TestSuite : INotifyPropertyChanged
    {
        static HttpClient Http = new HttpClient();

        public event PropertyChangedEventHandler PropertyChanged;

        public static async Task<TestSuite> DownloadTestSuite()
        {
            var index = await GetIndex();

            return new TestSuite(ExtractTestsFromIndex(index));
        }

        public List<SvgTest> Tests { get { return testsByChapter[selectedChapter]; } }
        public ICollection<string> Chapters { get { return testsByChapter.Keys;  } }

        public string SelectedChapter
        {
            get
            {
                return selectedChapter;
            }
            set
            {
                if (selectedChapter == value)
                    return;

                selectedChapter = value;
                SelectedTest = Tests[0];

                if (PropertyChanged != null)
                {
                    PropertyChanged(this, new PropertyChangedEventArgs("Tests"));
                }
            }
        }

        Dictionary<string, List<SvgTest>> testsByChapter;
        string selectedChapter;


        SvgTest selectedTest;
        public SvgTest SelectedTest
        {
            get { return selectedTest; }
            set
            {
                selectedTest = value;
                if (PropertyChanged != null)
                    PropertyChanged(this, new PropertyChangedEventArgs("SelectedTest"));
            }
        }

        protected TestSuite(List<SvgTest> tests)
        {
            testsByChapter = new Dictionary<string, List<SvgTest>>();

            foreach (var test in tests)
            {
                List<SvgTest> chapterTests;
                if (!testsByChapter.TryGetValue(test.Chapter, out chapterTests))
                {
                    chapterTests = new List<SvgTest>();
                    testsByChapter.Add(test.Chapter, chapterTests);
                }

                chapterTests.Add(test);
            }

            selectedChapter = Chapters.First();
            SelectedTest = Tests[0];

            BackgroundDownloadTestData();
        }

        static async Task<XmlDocument> GetIndex()
        {
            var indexUrl = "http://www.w3.org/Graphics/SVG/Test/20110816/harness/htmlObjectApproved/index.html";
            var buffer = await Http.GetBufferAsync(new Uri(indexUrl));

            var doc = new XmlDocument();
            doc.LoadXmlFromBuffer(buffer, new XmlLoadSettings() { ProhibitDtd = false });

            return doc;
        }

        static List<SvgTest> ExtractTestsFromIndex(XmlDocument doc)
        {
            List<SvgTest> tests = new List<SvgTest>();

            var links = doc.GetElementsByTagName("a");

            foreach (var link in links)
            {
                var svgName = link.InnerText;
                var newTest = new SvgTest(svgName);

                // We don't expect anything in these chapters to work
                if (newTest.Chapter == "animate" || newTest.Chapter=="interact" || newTest.Chapter=="script")
                    continue;

                // Filter out any tests that end with 'z' since we don't support
                // compressed files yet.
                // TODO; support svgz files
                if (newTest.SvgUri.EndsWith("svgz"))
                    continue;

                // Filter out any DOM tests; we don't support the DOM
                if (newTest.Name.Contains("-dom"))
                    continue;

                tests.Add(newTest);
            }

            return tests;
        }

        void BackgroundDownloadTestData()
        {
            Task.Run(async () =>
            {
                foreach (var test in Tests)
                {
                    Debug.WriteLine(string.Format("prefetch: {0}", test.SvgUri));
                    await CachedData.GetStorageFileAsync(new Uri(test.SvgUri));
                    Debug.WriteLine(string.Format("prefetch: {0}", test.ReferencePngUri));
                    await CachedData.GetStorageFileAsync(new Uri(test.ReferencePngUri));
                }
            });
        }    
    }

    public class DesignTimeTestSuite : TestSuite
    {
        public DesignTimeTestSuite()
            : base(CreateDesignTimeTests())
        {
        }

        static List<SvgTest> CreateDesignTimeTests()
        {
            var tests = new List<SvgTest>();
            for (int i = 0; i < 100; ++i)
            {
                tests.Add(new SvgTest(string.Format("test{0:000}.svg", i)));
            }
            return tests;
        }
    }
}
