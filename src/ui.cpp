#include <iostream>
#include <string>
#include <cmath>
#include <cstdint>
#include "imgui.h"
#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_opengl3.h"

std::string truncateText(const std::string &text, float maxWidth) {
	float textWidth = ImGui::CalcTextSize(text.c_str()).x;
	if (textWidth <= maxWidth) {
		return text;
	}

	std::string truncatedText = text;
	while (ImGui::CalcTextSize((truncatedText + "...").c_str()).x > maxWidth && !truncatedText.empty()) {
		truncatedText.pop_back();
	}
	return truncatedText + "...";
}
	
std::string convertTimeToString(double timeWithMilliseconds) {
	uint32_t timeInSeconds = static_cast<uint32_t>(timeWithMilliseconds);
	std::string finalTime;
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
