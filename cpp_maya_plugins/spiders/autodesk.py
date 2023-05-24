import scrapy
import json
import os
def extractLine(index, node):
    # print(node.extract())
    # child_nodes = node.xpath("*|text()")
    res = node.xpath("string(.)").extract_first()
    return res;

class AutodeskSpider(scrapy.Spider):
    name = "autodesk"
    allowed_domains = ["help.autodesk.com"]
    start_urls  = []
    url_to_file = dict();

    def __init__(self, name=None, **kwargs):
        super().__init__(name, **kwargs)
        with open('data.json') as f:
            data = json.load(f)
            for d in data:
                self.start_urls.append(d["url"])
                self.url_to_file[d["url"]] = d["file"]

    # start_urls = ["https://help.autodesk.com/cloudhelp/2022/ENU/Maya-SDK/cpp_ref/auto_loader_2auto_loader_8cpp-example.html"]

    def parse(self, response):
        code = response.xpath('//div[@class="contents"]/div[@class="fragment"]/div')
        res = '';
        for index, item in enumerate(code):
            res += extractLine(index, item) + '\n';
        
        url = response.request.url
        # print(url, res, self.url_to_file[url])
        dir = os.path.dirname(self.url_to_file[url])
        # print("dir = ", dir)
        if dir != '' and not os.path.exists(dir):
            os.makedirs(dir)
        with open(self.url_to_file[url], "w") as cf:
            cf.write(res)