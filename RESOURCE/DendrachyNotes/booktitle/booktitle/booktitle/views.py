import json

from django.http import HttpResponse, JsonResponse
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

def build_tree(book, books):
    """递归构建父子结构的树"""
    children = []
    for child in books:
        if child.pid == book.id:
            children.append(build_tree(child, books))
    return {
        'id': book.id,
        'pid': book.pid,
        'name': book.name,
        'title': book.title,
        'children': children
    }
@csrf_exempt
def book_title_list(request):
    books = BookTitle.objects.all()
    # 构建一个字典，方便查找
    book_dict = {book.id: book for book in books}
    # 只处理根节点（pid为0或None的项）
    root_books = [book for book in books if book.pid == 0 or book.pid is None]
    # 构建树形结构
    result = [build_tree(book, books) for book in root_books]
    return JsonResponse(result, safe=False)