# Project Notes

## Issues encountered and fixes
- unlinked main.cpp from the test build step (that was being used instead of the test framework main)

## Installing
I was using Windows, but had toswitch to WSL.

### Important steps:
- moved project directory from mnt/c/...../nand2tetris to \\wsl localhost\Ubuntu\home\rockclimber147\Projects (runs on the linux in wsl instead of windows file system, faster and better)
- installed g++ and gdb (I think I had them already from OS class)
- installed CMake extension and added a Cmake file