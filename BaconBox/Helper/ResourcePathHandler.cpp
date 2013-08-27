#include "BaconBox/Helper/ResourcePathHandler.h"

#include <fstream>

#include "BaconBox/PlatformFlagger.h"
#ifdef BB_IPHONE_PLATFORM
#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#elif defined(BB_MAC_PLATFORM) || defined(BB_LINUX)
#include <sys/types.h>
#include <pwd.h>
#include <unistd.h>
#include <sys/stat.h>

#include <sstream>
#elif defined(BB_QT)
#include <QDir>
#include <iostream>
#include <QCoreApplication>
#include <QDesktopServices>
#endif
#include "BaconBox/Console.h"
#include "BaconBox/Core/Engine.h"

namespace BaconBox {
	
	std::string ResourcePathHandler::debugResourcePath = "";


	std::string ResourcePathHandler::getResourcePathFor(const std::string &item) {
		std::string path;
#ifdef BB_IPHONE_PLATFORM
		NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

		NSString *resourceDirectory = [[NSBundle mainBundle] resourcePath];
	path = ((std::string)[resourceDirectory cStringUsingEncoding: NSASCIIStringEncoding] + "/" + item);
		[pool release];
#else
		if(debugResourcePath != ""){
			path = debugResourcePath + "/" + item;
		}
		else{
			path = Engine::getApplicationPath();
		

#ifdef BB_MAC_PLATFORM
		path = path + "/../Resources/" + item;
#else
		path = path + "/Resources/" + item;
#endif

#endif
		}
		return path;
	}

	void ResourcePathHandler::setDebugResourcePath(const std::string & path){
		debugResourcePath = path;
	}
	
	std::string ResourcePathHandler::getResourcePath(){
		std::string resourcePath;
#ifdef BB_IPHONE_PLATFORM

		NSString *resourceDirectory = [[NSBundle mainBundle] resourcePath];
		resourcePath = [resourceDirectory cStringUsingEncoding: NSASCIIStringEncoding];
#else
		
		if(debugResourcePath != "") return debugResourcePath;

resourcePath = Engine::getApplicationPath();
		
#ifdef BB_MAC_PLATFORM
		resourcePath = resourcePath + "/../Resources";
#else
		resourcePath = resourcePath + "/Resources";
#endif

#endif
		return resourcePath;
	}

	
	std::string ResourcePathHandler::getDocumentPathFor(const std::string &item) {
		std::string documentPath = getDocumentPath();
		documentPath.append(item);
		return documentPath;
	}

	std::string ResourcePathHandler::getDocumentPath() {
#ifdef BB_IPHONE_PLATFORM
		NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

		NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
		NSString *documentsDirectory = [paths objectAtIndex: 0];
	std::string documentPath = [documentsDirectory cStringUsingEncoding: NSASCIIStringEncoding];
		[pool release];
		return documentPath;
#elif defined(BB_QT)
		return QDesktopServices::storageLocation(QDesktopServices::DataLocation).toStdString();
#elif defined(BB_MAC_PLATFORM) || defined(BB_LINUX)
#ifdef BB_MAC_PLATFORM
		const std::string PATH = "/Library/Application Support/";
#else
		const std::string PATH = "/.config/";
#endif
		static bool firstTime = true;
		std::stringstream ss;

		ss << getpwuid(getuid())->pw_dir << PATH << Engine::getApplicationName() << "/";

		if (firstTime) {
			createFolder(ss.str());
			firstTime = false;
		}

		return ss.str();
#else
		return std::string();
#endif
	}

	bool ResourcePathHandler::createDocumentFolder(const std::string &path) {
		return createFolderTree(getDocumentPath() + path);
	}

	bool ResourcePathHandler::createFolder(const std::string &path) {
#ifdef BB_IPHONE_PLATFORM
		return false;
#elif defined(BB_MAC_PLATFORM) || defined(BB_LINUX)

		if (mkdir(path.c_str(), 0755)) {
			return true;

		} else {
			return false;
		}

#else
		return false;
#endif
	}

	bool ResourcePathHandler::createFolderTree(const std::string &path) {
		bool result = false;

		// We make sure the path isn't empty.
		if (!path.empty()) {
			std::string::const_iterator i = path.begin();
			std::string tmpPath;
			tmpPath.reserve(path.size());

#ifdef BB_WINDOWS_PLATFORM

			if (path.size() >= 2 && path.at(1) == ':' && ((path.at(0) >= 'A' && path.at(0) <= 'Z') || (path.at(0) >= 'a' && path.at(0) <= 'z'))) {
				tmpPath.append(path, 0, 2);
				++i;
				++i;
			}

#endif

			// For each character in the path.
			while (!result && i != path.end()) {
				tmpPath.append(1, *i);

				// If we encounter a slash and the folder doesn't already exist,
				// we try to create the folder.
				if (*i == '/' && !folderExists(tmpPath) && createFolder(tmpPath)) {
					Console::print("Failed to create the document folder \"");
					Console::print(tmpPath);
					Console::println("\".");
					result = true;
				}

				++i;
			}

			// If the path didn't end with a slash, we try to create the last
			// folder.
			if (!result && tmpPath.at(tmpPath.size() - 1) != '/' && !folderExists(tmpPath) && createFolder(tmpPath)) {
				Console::print("Failed to create the document folder \"");
				Console::print(tmpPath);
				Console::println("\".");
				result = true;
			}
		}

		return result;
	}

	bool ResourcePathHandler::folderExists(const std::string &path) {
#ifdef BB_IPHONE_PLATFORM
		return false;
#elif defined(BB_MAC_PLATFORM) || defined(BB_LINUX)
		struct stat st;
		return stat(path.c_str(), &st) == 0;
#else
		return false;
#endif
	}

	bool ResourcePathHandler::isFileReadable(const std::string &filePath) {
		// We try to open the file for reading.
		std::ifstream file(filePath.c_str());
		bool result = false;

		// If the file is open, it means it can be read.
		if (file.is_open()) {
			result = true;
			// We make sure to close the file.
			file.close();
		}

		return result;
	}
	
	std::string ResourcePathHandler::getPathFromFilename(std::string filename){
		std::string::size_type found = filename.find_last_of("/\\");
	    return filename.substr(0, found);
	}

	bool ResourcePathHandler::isFileWritable(const std::string &filePath) {
		// We try to open the file for writing.
		std::ofstream file(filePath.c_str());
		bool result = false;

		// If the file is open, it means it can be read.
		if (file.is_open()) {
			result = true;
			// We make sure to close the file.
			file.close();
		}

		return result;
	}
}
