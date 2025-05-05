from setuptools import find_packages
from setuptools import setup

setup(
    name='correct_controller',
    version='0.0.0',
    packages=find_packages(
        include=('correct_controller', 'correct_controller.*')),
)
