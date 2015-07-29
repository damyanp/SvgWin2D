using System;
using System.Collections.Generic;
using System.ComponentModel;
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

        public List<SvgTest> Tests { get; private set; }

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
            Tests = tests;
            SelectedTest = Tests[0];
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
                tests.Add(newTest);
            }

            // Filter the tests

            Func<SvgTest, bool> testFilter = test =>
                (test.Name.StartsWith("coords") && !test.Name.Contains("-dom-"))
                || test.Name.StartsWith("shapes")
                || test.Name.StartsWith("color")
                || test.Name.StartsWith("paths");

            tests = tests.Where(testFilter).ToList();

            return tests;
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
