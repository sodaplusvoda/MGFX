from setuptools import setup, Extension

mgfx_module = Extension(
	'mgfx',
	sources=['mgfx.c'],
	libraries=['glfw'],
)

setup(
	name='mgfx',
	version='0.1',
	description='Низкоуровневый движок на C для Python',
	ext_modules=[mgfx_module],
)