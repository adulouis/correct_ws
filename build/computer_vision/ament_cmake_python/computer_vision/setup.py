from setuptools import find_packages
from setuptools import setup

setup(
    name='computer_vision',
    version='0.0.0',
    packages=find_packages(
        include=('computer_vision', 'computer_vision.*')),
)
