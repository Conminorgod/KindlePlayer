#include <iostream>
#include <string>
#include <cmath>
#include <cstdint>
#include "imgui.h"
#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_opengl3.h"

// shortens a string to a specific length and adds "..." at the end
std::string truncateText(const std::string &text, float maxWidth) {
	float textWidth = ImGui::CalcTextSize(text.c_str()).x;
	if (textWidth <= maxWidth) {
		return text;
	}

	std::string truncatedText = text;
	while (ImGui::CalcTextSize((truncatedText + "...").c_str()).x > maxWidth && !truncatedText.empty()) {
		truncatedText.pop_back();
	}
	
	if (truncatedText.length() + 5 < text.length()) {
		truncatedText = truncatedText + "\n";
	
		uint32_t i = truncatedText.length();
		while (ImGui::CalcTextSize((truncatedText + "...").c_str()).x < maxWidth && i < text.length()) {
			truncatedText = truncatedText + text[i];
			i++;
		}
	}

	return truncatedText + "...";
}
	
// converts a double representing the amount of seconds passed to a string in the format "n:nn"
std::string convertTimeToString(double timeWithMilliseconds) {
	uint32_t timeInSeconds = static_cast<uint32_t>(timeWithMilliseconds);
	std::string finalTime;

	// I really don't like this. The if/else that both have nested if statements just irks me
	// But I don't know of a better way to do this
	if (timeInSeconds >= 60) {
		uint32_t numMinutes = timeInSeconds/60;
		uint32_t numSeconds = std::fmod(timeInSeconds, 60);
		if (numSeconds >= 10) {
			finalTime = std::to_string(numMinutes) + ":" + std::to_string(numSeconds);
		} else {
			finalTime = std::to_string(numMinutes) + ":0" + std::to_string(numSeconds);
		}
	} else {
		if (timeInSeconds >= 10) {
			finalTime = "0:" + std::to_string(timeInSeconds);
		} else {
			finalTime = "0:0" + std::to_string(timeInSeconds);
		}
	}
	return finalTime;
}

