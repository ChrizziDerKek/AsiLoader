#include <Windows.h>
#include <vector>
#include <Shlwapi.h>

struct Task {
	HANDLE fiber;
	uint64_t timer;
	void(*callback)();
};
std::vector<Task> tasks;
bool taskEditMutex = false;
Task* currentTask = NULL;
HANDLE scheduler = NULL;

void Run(Task& task) {
	if (GetTickCount64() < task.timer) {
		if (GetCurrentFiber() != scheduler)
			SwitchToFiber(scheduler);
		return;
	}
	if (task.fiber) {
		currentTask = &task;
		SwitchToFiber(currentTask);
		currentTask = NULL;
	}
	else {
		task.fiber = CreateFiber(NULL, [](void* handle) {
			((Task*)handle)->callback();
			}, &task);
	}
	SwitchToFiber(scheduler);
}

__declspec(dllexport) void TaskTick() {
	if (!scheduler)
		scheduler = IsThreadAFiber() ? GetCurrentFiber() : ConvertThreadToFiber(NULL);
	for (Task& t : tasks) {
		if (taskEditMutex)
			break;
		Run(t);
	}
}

__declspec(dllexport) void TaskCreate(void(*func)()) {
	taskEditMutex = true;
	Task t = { NULL, GetTickCount64(), func };
	tasks.push_back(t);
	taskEditMutex = false;
}

__declspec(dllexport) bool TaskRemove(void(*func)()) {
	taskEditMutex = true;
	for (auto it = tasks.begin(); it != tasks.end(); it++) {
		if (it->callback == func) {
			DeleteFiber(it->fiber);
			if (GetCurrentFiber() != scheduler)
				SwitchToFiber(scheduler);
			taskEditMutex = false;
			return true;
		}
	}
	taskEditMutex = false;
	return false;
}

__declspec(dllexport) void TaskSleep(DWORD ms) {
	if (GetCurrentFiber() != scheduler)
		SwitchToFiber(scheduler);
	currentTask->timer = GetTickCount64() + ms;
}