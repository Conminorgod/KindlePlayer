# Kindle's Music Player

This is a (currently) small and simple music player I've made. I created this because I am tired of music streaming services sucking !!! I have to use YouTube because half of the songs I listen to aren't elsewhere, and shuffle on YouTube is really bad. I also made this to learn more about C++ and just generally get more experience.

**This project is currently in it's beta phase. All that is implemented so far is the basic UI. I will make it look good and add more features later**

I can't ensure anything during the build process because I have not tested it outside of my machine. I know I'm doing the thing every overconfident programmer does, I will make it better later I swear.

## Dependencies
The only dependency is cmake

## Compatibility
- Linux
- Windows (hopefully)
- You might need to pray to some deity to get it working on other systems

## Features
- Next and previous song buttons
- Looping songs
- Playlists
- Shuffling and unshuffling playlists
- Exporting a YouTube playlist (*Requires yt-dlp*)
- Going forward/back 5 seconds
- Controls

## Building
In a terminal in the project directory:
```
mkdir build
cd build
cmake ..
cmake --build .
```

## Usage
Open the exporting window by hitting the export button. Copy a YouTube playlist link and place it in the "Enter Playlist Link" text box in the playlist window, then give it a name and hit export. Exporting will take a long time, especially for bigger playlists. The playlist probably won't be in the same order that it is on YouTube. Not sure anyone really cares about that but might as well just clarify. Click the button that has the playlist's name on it. Then click the song you want to listen to. 
