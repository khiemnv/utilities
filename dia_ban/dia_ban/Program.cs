// See https://aka.ms/new-console-template for more information
using System.Text.RegularExpressions;

// get all data
var url = @"https://japan-postcode.810popo.net/hokkaido/";

if (args.Length == 0)
{
    Console.WriteLine("cmd <perf,perf>");
    return;
}

HttpClient client = new HttpClient();
var arr = args[0].Split(new char[] { ',', ' ' });
var perfs = new List<Prefecture>();
foreach (var perf in arr)
{
    url = $@"https://japan-postcode.810popo.net/{perf}/";
    var p = new Prefecture();
    try
    {
        string responseBody = await Common.DownLoad(url, client);
        var n = Regex.Match(responseBody, @"ZIP code of (\w+) Prefecture");
        p.name = n.Groups[1].Value;
        var lst = Regex.Matches(responseBody, @"<li><a itemprop='url' href='(\w+\/)'><span itemprop='name'>(\w+)<\/span><\/a><\/li>", RegexOptions.Multiline);
        foreach (Match m in lst)
        {
            var url2 = url + m.Groups[1].Value;
            var name2 = m.Groups[2].Value;
            var city = new City { name = name2 };
            p.Cities.Add(city);

            string responseBody2 = await Common.DownLoad(url2, client);
            var lst2 = Regex.Matches(responseBody2, @"<span itemprop='name'>(\w+)</span>", RegexOptions.Multiline);
            foreach (Match m2 in lst2)
            {
                var name3 = m2.Groups[1].Value;
                var town = new City.Town { name = name3 };
                city.towns.Add(town);
            }
        }
        perfs.Add(p);
    }
    catch (HttpRequestException e)
    {
        Console.WriteLine("\nException Caught!");
        Console.WriteLine("Message :{0} ", e.Message);
    }
}

var path = string.Join('_', arr) + ".txt";
Common.printPefs(perfs, path);

return;


var file = @"D:\tmp\github\clb\data_hokkaido.txt";
var lines = File.ReadAllLines(file);
var s = 0;
var citys = new List<City>();
City curCity = new City();
foreach (var line in lines)
{
    switch (s)
    {
        case 0:
            var m0 = Regex.Match(line, @"All Towns and ZIP codes in (\w+)");
            if (m0.Success)
            {
                curCity = new City { name = m0.Groups[0].Value };
                citys.Add(curCity);
                break;
            }

            var m1 = Regex.Match(line, @"(\w+) : (〒\d+-\d+)");
            if (m1.Success)
            {
                curCity.towns.Add(new City.Town { name = m1.Groups[1].Value, code = m1.Groups[2].Value });
            }
            break;
    }
}


class Prefecture
{
    public string name = "";
    public List<City> Cities = new List<City>();
}
class City
{
    public class Town
    {
        public string name = "";
        public string code = "";
    }
    public string name = "";
    public List<Town> towns = new List<Town>();
}

class Common
{
    public static void printPefs(List<Prefecture> perfs, string path)
    {
        var lines = new List<string>();
        var n = 1;
        perfs.ForEach(p =>
        {
            lines.Add($"{n}. Tỉnh {p.name}");
            foreach (var city in p.Cities)
            {
                var txt = string.Join(", ", city.towns.Select(x => "Thị trấn " + x.name).ToList());
                lines.Add($"  + Thành phố {city.name}: {txt}");
            }
            n++;
        });
        File.WriteAllLines(path, lines);
    }
    public static void printCitys(List<City> cities, string path)
    {
        // print
        var fout = File.CreateText(path);
        foreach (var city in cities)
        {
            var txt = string.Join(", ", city.towns.Select(x => "Thị trấn " + x.name).ToList());
            fout.WriteLine($"+ Thành phố {city.name}: {txt}");
        }
        fout.Close();
    }
    public static async Task<string> DownLoad(string url, HttpClient client, string tmpDir = "download")
    {
        if (!Directory.Exists(tmpDir)) { Directory.CreateDirectory(tmpDir); }
        var fileName = new Uri(url).LocalPath.Trim('/').Replace("/", "_") + ".html";
        var filePath = Path.Combine(tmpDir, fileName);
        string responseBody = "";
        if (!File.Exists(filePath))
        {
            using HttpResponseMessage response = await client.GetAsync(url);
            response.EnsureSuccessStatusCode();
            responseBody = await response.Content.ReadAsStringAsync();
            File.WriteAllText(filePath, responseBody);
        }
        else
        {
            responseBody = File.ReadAllText(filePath);
        }

        return responseBody;
    }
}
