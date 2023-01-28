#!/bin/bash

trap "exit" INT
if [ "$HOME" = "/home/user" ]; then
	if [ -d "build-debug" ]; then
		echo -e "\e[1;94mBuild: powder, font, debug\e[0m"
		(
			cd build-debug/
			ninja
		)
	fi
	if [ -d "build-release-static" ]; then
		echo -e "\e[1;94mBuild: powder, font, release, static\e[0m"
		(
			cd build-release-static/
			ninja
		)
	fi
	if [ -d "powder-build-debug" ]; then
		echo -e "\e[1;94mBuild: powder, debug\e[0m"
		(
			cd powder-build-debug/
			ninja
		)
	fi
	if [ -d "powder-build-release-static" ]; then
		echo -e "\e[1;94mBuild: powder, release, static\e[0m"
		(
			cd powder-build-release-static/
			ninja
		)
	fi
	if [ -d "font-build-debug" ]; then
		echo -e "\e[1;94mBuild: font, debug\e[0m"
		(
			cd font-build-debug/
			ninja
		)
	fi
	if [ -d "font-build-release-static" ]; then
		echo -e "\e[1;94mBuild: font, release, static\e[0m"
		(
			cd font-build-release-static/
			ninja
		)
	fi
else
	echo "Not running outside container"
fi
