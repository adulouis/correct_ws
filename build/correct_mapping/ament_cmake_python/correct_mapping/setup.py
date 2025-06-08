from setuptools import find_packages
from setuptools import setup

setup(
    name='correct_mapping',
    version='0.0.0',
    packages=find_packages(
        include=('correct_mapping', 'correct_mapping.*')),
)
