#pragma once

#define NO_COPY_MOVE(classname) \
	classname(const classname&) = delete; \
	classname(classname&&) = delete; \
	void operator=(const classname&) = delete; \
	void operator=(classname&&) = delete
