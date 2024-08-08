from django.db import models

# Create your models here.
# models.py


class BookTitle(models.Model):
    id = models.IntegerField(primary_key=True)
    pid = models.IntegerField()
    name = models.CharField(max_length=200)
    title = models.CharField(max_length=200)