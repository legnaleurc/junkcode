from datetime import date
import unittest

import wcpan.model as wm


class ClassCreationTestCase(unittest.TestCase):

    def setUp(self):
        class SimpleModel(wm.Model):
            name = wm.StringField()
            field_with_source = wm.StringField(source='foo')
        self.model_class = SimpleModel
        self.instance = SimpleModel()

    def test_class_created(self):
        """Model instance should be of type SimpleModel"""
        self.assertTrue(isinstance(self.instance, self.model_class))

    def test_fields_created(self):
        """Model instance should have a property called _fields"""
        self.assertTrue(hasattr(self.instance, '_fields'))

    def test_field_collected(self):
        """Model property should be of correct type"""
        self.assertTrue(isinstance(self.instance._fields['name'], wm.StringField))

    def test_field_source_not_set(self):
        """Field without a custom source should have a source of None"""
        self.assertEqual(self.instance._fields['name'].source, None)

    def test_field_source_set(self):
        """Field with custom source specificied should have source property set correctly"""
        self.assertEqual(self.instance._fields['field_with_source'].source, 'foo')


class ModelTestCase(unittest.TestCase):

    def setUp(self):
        class Person(wm.Model):
            name = wm.StringField()
            age = wm.IntegerField()

        self.Person = Person
        self.data = {'name': 'Eric', 'age': 18}

    def test_model_creation(self):
        instance = self.Person.from_dict(self.data)
        self.assertTrue(isinstance(instance, wm.Model))
        self.assertEqual(instance.name, self.data['name'])
        self.assertEqual(instance.age, self.data['age'])

    def test_model_reserialization(self):
        instance = self.Person.from_dict(self.data)
        instance.name = 'John'
        self.assertEqual(instance.to_dict()['name'],
                         'John')

    def test_model_type_change_serialization(self):
        class Event(wm.Model):
            time = wm.DateField(format="%Y-%m-%d")

        data = {'time': '2000-10-31'}

        instance = Event.from_dict(data)
        output = instance.to_dict(serial=True)
        self.assertEqual(output['time'], instance.time.isoformat())

    def test_model_add_field(self):
        obj = self.Person.from_dict(self.data)
        obj.add_field('gender', 'male', wm.StringField())
        self.assertEqual(obj.gender, 'male')
        self.assertEqual(obj.to_dict(), dict(self.data, gender='male'))

    def test_model_late_assignment(self):
        instance = self.Person.from_dict(dict(name='Eric'))
        self.assertEqual(instance.to_dict(), dict(name='Eric'))
        instance.age = 18
        self.assertEqual(instance.to_dict(), self.data)
        instance.name = 'John'
        self.assertEqual(instance.to_dict(), dict(name='John', age=18))
        instance.age = '19'
        self.assertEqual(instance.to_dict(), dict(name='John', age=19))

        format = '%m-%d-%Y'
        today = date.today()
        today_str = today.strftime(format)

        instance.add_field('birthday', today_str,
                           wm.DateField(format))
        self.assertEqual(instance.to_dict()['birthday'], today)
        instance.birthday = today
        self.assertEqual(instance.to_dict()['birthday'], today)
