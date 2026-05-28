#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <string.h>

#define MAX_WINDOWS 10

//Структура для связывания строкового ID с реальным хэндлом окна GLFW
typedef struct {
	char id[128];
	GLFWwindow* handle;
} WindowEntry;

static WindowEntry window_registry[MAX_WINDOWS];
static int window_count = 0;

//Добавление окна в реестр
static void register_window(const char* id, GLFWwindow* handle) {
	if (window_count < MAX_WINDOWS) {
		strncpy(window_registry[window_count].id, id, 127);
		window_registry[window_count].handle = handle;
		window_count++;
	}
}

static GLFWwindow* find_window(const char* id) {
	for (int i = 0; i < window_count; i++) {
		if (strcmp(window_registry[i].id, id) == 0) {
			return window_registry[i].handle;
		}
	}
	return NULL;
}

static PyObject* mgfx_create_window(PyObject* self, PyObject* args) {
	const char* title;
	const char* win_id;
	int width;
	int height;

	//Парсим аргументы из Python: s - string, s - string, i - int, i - int
	if (!PyArg_ParseTuple(args, "ssii", &title, &win_id, &width, &height)) {
		return NULL; 
	}

	//Инициализация GLFW при первом вызове
	static int glfw_initialized = 0;
	if (!glfw_initialized) {
		if (!glfwInit()) {
			PyErr_SetString(PyExc_RuntimeError, "GLFW failed to initialize");
			return NULL;
		}
		glfw_initialized = 1;
	}

	//Задаем настройки OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//Создаем само окно
	GLFWwindow* window = glfwCreateWindow(width, height, title, NULL, NULL);
	if (!window) {
		PyErr_SetString(PyExc_RuntimeError, "GLFW failed to create window");
		return NULL;
	}

	glfwMakeContextCurrent(window);

	//Регистрируем его под переданным ID
	register_window(win_id, window);

	Py_RETURN_NONE; //В Python вернет None
}

static PyObject* mgfx_is_open(PyObject* self, PyObject* args) {
	const char* win_id;
	if (!PyArg_ParseTuple(args, "s", &win_id)) return NULL;

	GLFWwindow* window = find_window(win_id);
	if (!window) {
		PyErr_SetString(PyExc_KeyError, "There is no window with this ID");
		return NULL;
	}

	if (!glfwWindowShouldClose(window)) {
		Py_RETURN_TRUE;
	} else {
		Py_RETURN_FALSE;
	}
}

static PyObject* mgfx_update(PyObject* self, PyObject* args) {
	const char* win_id;
	if (!PyArg_ParseTuple(args, "s", &win_id)) return NULL;

	GLFWwindow* window = find_window(win_id);
	if (!window) {
		Py_RETURN_NONE;
	}

	if (glfwWindowShouldClose(window)) {
		glfwDestroyWindow(window);
	} else {
		glfwMakeContextCurrent(window);
		glfwSwapBuffers(window);
	}

	glfwPollEvents();
	Py_RETURN_NONE;
}

static PyObject* mgfx_terminate(PyObject* self, PyObject* args) {
	glfwTerminate();
	Py_RETURN_NONE;
}

static PyMethodDef MgfxMethods[] = {
	{"create_window", mgfx_create_window, METH_VARARGS, "Создает и регистрирует окно"},
	{"is_open",       mgfx_is_open,       METH_VARARGS, "Проверяет, открыто ли окно"},
	{"update",        mgfx_update,        METH_VARARGS, "Обновляет экран окна по ID"},
	{"terminate",     mgfx_terminate,     METH_NOARGS,  "Закрывает GLFW"},
	{NULL, NULL, 0, NULL}
};

static struct PyModuleDef mgfxmodule = {
	PyModuleDef_HEAD_INIT,
	"mgfx",
	"Низкоуровневая графическая библиотека на Си",
	-1,
	MgfxMethods
};

//import mgfx
PyMODINIT_FUNC PyInit_mgfx(void) {
	return PyModule_Create(&mgfxmodule);
}