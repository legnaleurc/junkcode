from datetime import date
import unittest

import wcpan.model as wm


class BaseFieldTestCase(unittest.TestCase):

    def test_field_without_provided_source(self):
        """If no source parameter is provided, the field's source attribute should be None"""
        field = wm.fields.BaseField()
        self.assertTrue(hasattr(field, 'source'))
        self.assertTrue(field.source is None)

    def test_field_with_provided_source(self):
        """If a source parameter is provided, the field's source attribute should be set to the value of this parameter"""
        field = wm.fields.BaseField(source='customsource')
        self.assertEqual(field.source, 'customsource')


class StringFieldTestCase(unittest.TestCase):

    def setUp(self):
        self.field = wm.StringField()

    def test_string_conversion(self):
        self.field.populate('somestring')
        self.assertEqual(self.field.to_python(), 'somestring')

    def test_none_conversion(self):
        """StringField should convert None to empty string"""
        self.field.populate(None)
        self.assertEqual(self.field.to_python(), '')


class IntegerFieldTestCase(unittest.TestCase):

    def setUp(self):
        self.field = wm.IntegerField()

    def test_integer_conversion(self):
        self.field.populate(123)
        self.assertEqual(self.field.to_python(), 123)

    def test_float_conversion(self):
        self.field.populate(123.4)
        self.assertEqual(self.field.to_python(), 123)

    def test_string_conversion(self):
        self.field.populate('123')
        self.assertEqual(self.field.to_python(), 123)

    def test_none_conversion(self):
        """IntegerField should convert None to 0"""
        self.field.populate(None)
        self.assertEqual(self.field.to_python(), 0)


class FloatFieldTestCase(unittest.TestCase):

    def setUp(self):
        self.field = wm.FloatField()

    def test_float_conversion(self):
        self.field.populate(123.4)
        self.assertEqual(self.field.to_python(), 123.4)

    def test_integer_conversion(self):
        self.field.populate(123)
        self.assertEqual(self.field.to_python(), 123.0)

    def test_string_conversion(self):
        self.field.populate('123.4')
        self.assertEqual(self.field.to_python(), 123.4)

    def test_none_conversion(self):
        """FloatField should convert None to 0.0"""
        self.field.populate(None)
        self.assertEqual(self.field.to_python(), 0.0)


class BooleanFieldTestCase(unittest.TestCase):

    def setUp(self):
        self.field = wm.BooleanField()

    def test_true_conversion(self):
        self.field.populate(True)
        self.assertEqual(self.field.to_python(), True)

    def test_false_conversion(self):
        self.field.populate(False)
        self.assertEqual(self.field.to_python(), False)

    def test_string_conversion(self):
        """BooleanField should convert the string "True" (case insensitive) to True, all other values to False"""
        self.field.populate('true')
        self.assertEqual(self.field.to_python(), True)
        self.field.populate('True')
        self.assertEqual(self.field.to_python(), True)
        self.field.populate('False')
        self.assertEqual(self.field.to_python(), False)
        self.field.populate('asdfasfasfd')
        self.assertEqual(self.field.to_python(), False)

    def test_integer_conversion(self):
        """BooleanField should convert values <= 0 to False, all other integers to True"""
        self.field.populate(0)
        self.assertEqual(self.field.to_python(), False)
        self.field.populate(-100)
        self.assertEqual(self.field.to_python(), False)
        self.field.populate(100)
        self.assertEqual(self.field.to_python(), True)


class DateTimeFieldTestCase(unittest.TestCase):

    def setUp(self):
        self.format = "%a %b %d %H:%M:%S +0000 %Y"
        self.datetimestring = "Tue Mar 21 20:50:14 +0000 2006"
        self.field = wm.DateTimeField(format=self.format)

    def test_format_conversion(self):
        import datetime
        self.field.populate(self.datetimestring)
        converted = self.field.to_python()
        self.assertTrue(isinstance(converted, datetime.datetime))
        self.assertEqual(converted.strftime(self.format), self.datetimestring)

    def test_iso8601_conversion(self):
        import datetime

        field = wm.DateTimeField()
        field.populate("2010-07-13T14:01:00Z")
        result = field.to_python()
        expected = datetime.datetime(2010, 7, 13, 14, 1, 0,
                                     tzinfo=datetime.timezone.utc)
        self.assertEqual(expected, result)


        field = wm.DateTimeField()
        field.populate("2010-07-13T14:02:00-05:00")
        result = field.to_python()
        offset = datetime.timedelta(hours=-5)
        expected = datetime.datetime(2010, 7, 13, 14, 2, 0,
                                     tzinfo=datetime.timezone(offset))

        self.assertEqual(expected, result)


        field = wm.DateTimeField()
        field.populate("20100713T140200-05:00")
        result = field.to_python()
        expected = datetime.datetime(2010, 7, 13, 14, 2, 0,
                                     tzinfo=datetime.timezone(offset))

        self.assertEqual(expected, result)


    def test_iso8601_to_serial(self):
        import datetime

        field = wm.DateTimeField()
        field.populate("2010-07-13T14:01:00Z")
        native = field.to_python()
        expected = "2010-07-13T14:01:00+00:00"
        result = field.to_serial(native)

        self.assertEqual(expected, result)

        field = wm.DateTimeField()
        field.populate("2010-07-13T14:02:00-05:00")
        native = field.to_python()
        expected = "2010-07-13T14:02:00-05:00"
        result = field.to_serial(native)

        self.assertEqual(expected, result)


class DateFieldTestCase(unittest.TestCase):

    def setUp(self):
        self.format = "%Y-%m-%d"
        self.datestring = "2010-12-28"
        self.field = wm.DateField(format=self.format)

    def test_format_conversion(self):
        import datetime
        self.field.populate(self.datestring)
        converted = self.field.to_python()
        self.assertTrue(isinstance(converted, datetime.date))
        self.assertEqual(converted.strftime(self.format), self.datestring)

    def test_iso8601_conversion(self):
        import datetime
        field = wm.DateField()
        field.populate("2010-12-28")
        result = field.to_python()
        expected = datetime.date(2010,12,28)
        self.assertEqual(expected, result)

        field = wm.DateField()
        field.populate("20101228")
        result = field.to_python()
        expected = datetime.date(2010, 12, 28)
        self.assertEqual(expected, result)


class TimeFieldTestCase(unittest.TestCase):

    def setUp(self):
        self.format = "%H:%M:%S"
        self.timestring = "09:33:30"
        self.field = wm.TimeField(format=self.format)

    def test_format_conversion(self):
        import datetime
        self.field.populate(self.timestring)
        converted = self.field.to_python()
        self.assertTrue(isinstance(converted, datetime.time))
        self.assertEqual(converted.strftime(self.format), self.timestring)

    def test_iso8601_conversion(self):
        import datetime
        field = wm.TimeField()
        field.populate("09:33:30")
        result = field.to_python()
        expected = datetime.time(9, 33, 30)
        self.assertEqual(expected, result)

        field = wm.TimeField()
        field.populate("093331")
        result = field.to_python()
        expected = datetime.time(9, 33, 31)
        self.assertEqual(expected, result)


class InstanceTestCase(unittest.TestCase):

    def test_basic_data(self):
        class ThreeFieldsModel(wm.Model):
            first = wm.StringField()
            second = wm.StringField()
            third = wm.StringField()

        data = {'first': 'firstvalue', 'second': 'secondvalue'}
        instance = ThreeFieldsModel.from_dict(data)

        self.assertEqual(instance.first, data['first'])
        self.assertEqual(instance.second, data['second'])

    def test_custom_data_source(self):
        class CustomSourceModel(wm.Model):
            first = wm.StringField(source='custom_source')

        data = {'custom_source': 'somevalue'}
        instance = CustomSourceModel.from_dict(data)

        self.assertEqual(instance.first, data['custom_source'])


class ModelFieldTestCase(unittest.TestCase):

    def test_model_field_creation(self):
        class IsASubModel(wm.Model):
            first = wm.StringField()

        class HasAModelField(wm.Model):
            first = wm.ModelField(IsASubModel)

        data = {'first': {'first': 'somevalue'}}
        instance = HasAModelField.from_dict(data)
        self.assertTrue(isinstance(instance.first, IsASubModel))
        self.assertEqual(instance.first.first, data['first']['first'])

    def test_model_field_to_serial(self):
        class User(wm.Model):
            name = wm.StringField()

        class Post(wm.Model):
            title = wm.StringField()
            author = wm.ModelField(User)

        data = {'title': 'Test Post', 'author': {'name': 'Eric Martin'}}
        post = Post.from_dict(data)
        self.assertEqual(post.to_dict(serial=True), data)

    def test_related_name(self):
        class User(wm.Model):
            name = wm.StringField()

        class Post(wm.Model):
            title = wm.StringField()
            author = wm.ModelField(User, related_name="post")

        data = {'title': 'Test Post', 'author': {'name': 'Eric Martin'}}
        post = Post.from_dict(data)
        self.assertEqual(post.author.post, post)
        self.assertEqual(post.to_dict(serial=True), data)

    def test_failing_modelfield(self):
        class SomethingExceptional(Exception):
            pass

        class User(wm.Model):
            name = wm.StringField()

            @classmethod
            def from_dict(cls, *args, **kwargs):
                raise SomethingExceptional("opps.")

        class Post(wm.Model):
            title = wm.StringField()
            author = wm.ModelField(User)

        data = {'title': 'Test Post', 'author': {'name': 'Eric Martin'}}
        self.assertRaises(SomethingExceptional, Post.from_dict,
                          data)


class ModelCollectionFieldTestCase(unittest.TestCase):

    def test_model_collection_field_creation(self):
        class IsASubModel(wm.Model):
            first = wm.StringField()

        class HasAModelCollectionField(wm.Model):
            first = wm.ModelCollectionField(IsASubModel)

        data = {'first': [{'first': 'somevalue'}, {'first': 'anothervalue'}]}
        instance = HasAModelCollectionField.from_dict(data)
        self.assertTrue(isinstance(instance.first, list))
        for item in instance.first:
            self.assertTrue(isinstance(item, IsASubModel))
        self.assertEqual(instance.first[0].first, data['first'][0]['first'])
        self.assertEqual(instance.first[1].first, data['first'][1]['first'])

    def test_model_collection_field_with_no_elements(self):
        class IsASubModel(wm.Model):
            first = wm.StringField()

        class HasAModelCollectionField(wm.Model):
            first = wm.ModelCollectionField(IsASubModel)

        data = {'first': []}
        instance = HasAModelCollectionField.from_dict(data)
        self.assertEqual(instance.first, [])

    def test_model_collection_to_serial(self):
        class Post(wm.Model):
            title = wm.StringField()

        class User(wm.Model):
            name = wm.StringField()
            posts = wm.ModelCollectionField(Post)

        data = {
                'name': 'Eric Martin',
                'posts': [
                            {'title': 'Post #1'},
                            {'title': 'Post #2'}
                ]
        }

        eric = User.from_dict(data)
        processed = eric.to_dict(serial=True)
        self.assertEqual(processed, data)

    def test_related_name(self):
        class Post(wm.Model):
            title = wm.StringField()

        class User(wm.Model):
            name = wm.StringField()
            posts = wm.ModelCollectionField(Post, related_name="author")

        data = {
                'name': 'Eric Martin',
                'posts': [
                            {'title': 'Post #1'},
                            {'title': 'Post #2'}
                ]
        }

        eric = User.from_dict(data)
        processed = eric.to_dict(serial=True)
        for post in eric.posts:
            self.assertEqual(post.author, eric)

        self.assertEqual(processed, data)


class FieldCollectionFieldTestCase(unittest.TestCase):

    def test_field_collection_field_creation(self):
        class HasAFieldCollectionField(wm.Model):
            first = wm.FieldCollectionField(wm.StringField())

        data = {'first': ['one', 'two', 'three']}
        instance = HasAFieldCollectionField.from_dict(data)
        self.assertTrue(isinstance(instance.first, list))
        self.assertTrue(len(data['first']), len(instance.first))
        for index, value in enumerate(data['first']):
            self.assertEqual(instance.first[index], value)

    def test_field_collection_field_to_serial(self):
        class Person(wm.Model):
            aliases = wm.FieldCollectionField(wm.StringField())
            events = wm.FieldCollectionField(wm.DateField('%Y-%m-%d',
                                        serial_format='%m-%d-%Y'), source='schedule')

        data = {
                    'aliases': ['Joe', 'John', 'Bob'],
                    'schedule': ['2011-01-30', '2011-04-01']
        }

        p = Person.from_dict(data)
        serial = p.to_dict(serial=True)
        self.assertEqual(serial['aliases'], data['aliases'])
        self.assertEqual(serial['events'][0], '01-30-2011')
