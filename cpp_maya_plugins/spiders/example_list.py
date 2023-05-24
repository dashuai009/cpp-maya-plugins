import scrapy
from cpp_maya_plugins.items import UrlItem

class ExampleListSpider(scrapy.Spider):
    name = "example_list"
    allowed_domains = ["help.autodesk.com"]
    start_urls = ["https://help.autodesk.com/cloudhelp/2022/ENU/Maya-SDK/cpp_ref/examples.html"]

    def parse(self, response):
        li = response.xpath('//ul/li/a[@class="el"]')
        res = []
        for i in li:
            item = UrlItem()
            item["url"] = "https://help.autodesk.com/cloudhelp/2022/ENU/Maya-SDK/cpp_ref/" + i.attrib['href']
            item["file"] = i.xpath("text()").extract_first()
            res.append(item)
            # break;
        return res