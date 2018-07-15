#include <string.h>
#include <ctype.h>
#include <stdbool.h>

char *join_strings(char* strings[], char* seperator, int count) {
	// Calculate total length of joined strings
	size_t total_length = 0;
	for (int i = 0; i < count; i++) total_length += strlen(strings[i]);
	total_length += strlen(seperator) * (count - 1); // For seperators
	total_length += 1; // For the terminator

	// Allocate memory for joined strings
	char* str = malloc(total_length);

	// Append all the strings
	str[0] = '\0'; // Empty string we can append to
	for (int i = 0; i < count; i++) {
		strcat(str, strings[i]);
		if (i < (count - 1)) strcat(str, seperator);
	}

	return str;
}

bool is_string_all_space(char* string) {
	for (int i = 0; i < strlen(string); i++) {
		if (!isspace(string[i])) {
			return false;
		}
	}
	return true;
}
