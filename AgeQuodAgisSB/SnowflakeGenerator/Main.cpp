#include "SFML/Graphics.hpp"
#include <Windows.h>
#include <vector>
#include <iostream>

// The SnowflakeGenerator takes in a directory as input and makes resulting Snowflakes from it
// Each file within the input directory is a 1/6th slice that is mirrored and repeated six times

// Finds all files to work with
std::vector<std::string> getFileNames(std::string inputDirectory) {
	// Find all image names in directory
	WIN32_FIND_DATAA fd;
	std::string inputDirectorySearch = inputDirectory + "*.png";
	HANDLE hFind = FindFirstFileA(inputDirectorySearch.c_str(), &fd);
	std::vector<std::string> fileNames;

	do {
		fileNames.push_back(fd.cFileName);
	} while (FindNextFileA(hFind, &fd));
	FindClose(hFind);

	return fileNames;
}

// Outputs snowflake to specified directory
void renderSnowflake(std::string fileName, std::string inputDirectory, std::string destinationDirectory) {
	sf::RenderTexture render;
	sf::Vector2f imageSize(1000.0f, 1000.0f);
	sf::Vector2f midSize(imageSize.x / 2, imageSize.y / 2);
	render.create(imageSize.x, imageSize.y);

	sf::Image snowflakeImage;
	std::string inputPath = inputDirectory + fileName;
	snowflakeImage.loadFromFile(inputPath);

	sf::Texture snowflakeTexture;
	snowflakeTexture.loadFromImage(snowflakeImage);

	sf::Texture snowflakeTextureFlipped;
	snowflakeImage.flipHorizontally();
	snowflakeTextureFlipped.loadFromImage(snowflakeImage);

	sf::Sprite snowflakeSprite;
	snowflakeSprite.setTexture(snowflakeTexture);
	snowflakeSprite.setOrigin(midSize);
	snowflakeSprite.setPosition(midSize);

	sf::Sprite snowflakeSpriteFlipped;
	snowflakeSpriteFlipped.setTexture(snowflakeTextureFlipped);
	snowflakeSpriteFlipped.setOrigin(midSize);
	snowflakeSpriteFlipped.setPosition(midSize);

	for (int i = 0; i < 6; ++i) {
		render.draw(snowflakeSprite);
		render.draw(snowflakeSpriteFlipped);

		snowflakeSprite.rotate(60);
		snowflakeSpriteFlipped.rotate(60);
	}

	render.display();

	std::string destinationPath = destinationDirectory + fileName;
	render.getTexture().copyToImage().saveToFile(destinationPath);
}

int main() {
	std::string inputDirectory = R"(Snowflake Sides\)";
	std::vector<std::string> fileNames = getFileNames(inputDirectory);

	std::string destinationPath = R"(C:\Users\Wax Chug da Gwad\AppData\Local\osu!\Songs\409783 void - Age quod agis\Snowflakes\)";
	for (auto fileName : fileNames) {
		std::cout << "Processing: " << fileName << std::endl;
		renderSnowflake(fileName, inputDirectory, destinationPath);
	}

	std::cout << "Finished generation" << std::endl;
	std::cin.get();
}