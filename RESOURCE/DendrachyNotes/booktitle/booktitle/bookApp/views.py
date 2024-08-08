import json

from django.http import HttpResponse
from django.shortcuts import render

# Create your views here.
# 数据库操作
from django.views.decorators.csrf import csrf_exempt

from bookApp.models import BookTitle


def queryByParent(request):
    array = BookTitle.objects.filter(pid=0).values()
    result = []
    for item in array:
        result.append(item)

    return HttpResponse(json.dumps(result), content_type='application/json')



@csrf_exempt
def addBook(request):
    # GET参数
    if request.method == 'POST':
        # POST参数
        pid = request.GET.get('pid')
        name = request.GET.get('name')
        title = request.GET.get('title')
        BookTitle.objects.create(pid=pid,name=name,title=title)
        # 文件上传
        # 返回简单响应
        return HttpResponse('新增成功')


@csrf_exempt
def updateBook(request):
    if request.method == 'POST':
        id = request.GET.get('id')
        name = request.GET.get('name')
        title = request.GET.get('title')
        pid = request.GET.get('pid')
        obj = BookTitle(id=id,name=name,title=title,pid=pid)
        obj.save()
        return HttpResponse('修改成功')

@csrf_exempt
def queryBook(request):
    # GET参数
    if request.method == 'POST':
        # POST参数
        id = request.GET.get('id')
        resultArray = []
        array = BookTitle.objects.filter(pid=id).values()
        for item in array:
            resultArray.append(item)
        obj = BookTitle.objects.get(id=id)
        result = {}
        result['name'] = obj.name
        result['title'] = obj.title
        result['sonList'] = resultArray
        # 文件上传
        # 返回简单响应
        return HttpResponse(json.dumps(result),content_type='application/json')

@csrf_exempt
def deleteBook(request):
    # GET参数
    if request.method == 'POST':
        # POST参数
        id = request.GET.get('id')
        obj = BookTitle.objects.get(id=id);
        obj.delete()
        return HttpResponse('删除成功')