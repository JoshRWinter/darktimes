#ifndef DARKTIMES_HPP
#define DARKTIMES_HPP

#define NOCOPYMOVE(classname) \
	classname(const classname&) = delete; \
	classname(classname&&) = delete; \
	void operator=(const classname&) = delete; \
	void operator=(classname&&) = delete

#endif
