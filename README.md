# draax

Version 0.1alpha

## WHY?

I suppose, in hindsight, I really didn't need this program. I had never logged in to my audible account through their website until very recently. I use an android app to listen to audiobooks so I assumed that all audible files were in the .aax format. As it turns out, you can log in to your audible account, download .aa files for any book that you own, and play them locally without needing the activation bytes.

In summary, if you have legally purchased an audiobook and have access to your audible account, YOU DO NOT NEED THIS SOFTWARE! :) Unfortunately I only made this discovery after I had already invested time in developing this application. Therefore, in an attempt to quickly finalize a working demo, I dramatically scaled back the planned feature set. Shortcuts were taken, blocking code was used (which will make it appear as if your interface has frozen), and there is no progress indicator of any kind (although console output does specify the iteration). This is a fully functional demo but I doubt that any additional time will be spent developing this app.

Please consider this application as an interface only. It does NOT extract meta data, validate activation bytes or transcode any file. It is merely an interface through which you can control external, 3rd party, console applications. At this time transcoding is not possible through draax. All aax files are already encoded in a very low quality, lossy format. It doesn't make sense to encode the audio again, reducing quality further, into another lossy format (mp3, vorbis, etc). This application ONLY copies the existing audio stream, preserving its quality, into a new container format. You can choose the desired output container in the options window.

At this time it is only supported on Linux but with minimal effort it could be ported to Windows and Mac as long as the applications below are available on those platforms.

## REQUIRED

* exiftool
* ffprobe
* ffmpeg

## OPTIONAL

* atomicparsley
* rcrack

## IMPORTANT!

DO NOT USE THIS SOFTWARE FOR ILLEGAL PURPOSES!

Please do not ask for support with the rcrack feature. No help will be provided.

If you have a legitimate need to use that feature you can activate it with this option. Make sure rtc files are contained within the same folder.

draax -r /path/to/rcrack

## DEVELOPERS

You can enable additional toolbar buttons in order to step through the conversion process one function at a time. Begin at the top left button and click each one sequentially from left to right.

draax -d

## FUTURE

Unlikely to be implemented by myself, but for any interested developers...

* Add support for Win and Mac. Will need to add .lnk to symlink names for Win systems. Code used to move files to Backups folder might need to be modified for Win.
* Use non-blocking code on QProcess.
* Require confirmation after layout is generated when using the wizard. Allow users to proofread/edit the authors/narrators/titles and then click a confirm/next button to continue with the encoding process.
* Add progress dialog. During ffmpeg process read standard output as it becomes available. Regex match the timestamp and convert to seconds. Compare to total duration in seconds. Use those values to determine percentage/position of progress indicator.
* Compare duration of input and output files after ffmpeg conversion to determine success.
* Improve error handling. Display popup alerts when an error is encountered.
* Make sure each dev button function is stable and can run independently without the wizard and without following a sequential order of functions (i.e., doesn't require that you click each toolbar button in order from left to right).
* Test audio codec before generating output. If incompatible with selected output container then dislay warning and exit.
* Add output codec menu to settings window.
* Enable users to transcode the aac stream by selecting different output codec. (vorbis, libmp3lame, flac, opus)
* Add checkbox to settings window labeled "Split output into chapters" and use ffmpeg to extract audio in chapter segments.
* Add support for .aa files and enable users to perform all of the same functions. Extract to different container, transcode, organize into their titles/authors/narrators folders, split into chapter segments.
* Embed cover image in all output files as meta tag. Use tageditor instead of atomicparsley.
* Allow editing of narrator and author names in tree widget. Add "| Qt::ItemIsEditable" to each author/narrator item and change toggleButtonAccess() to test ItemRole of each selected item. If any != AaxFile then disable remove button.
* Allow editing of titles in tree widget. Add checksum (from ffprobe) to each title item to maintain association if one is changed. Each title appears 3 times in the tree (title folder, narrator symlink, author symlink). Add event to detect changes to tree items. If changed item is title then change all items with the same checksum to new title. Title of original aax file ->text(5) must be changed as well.
* Allow users to remove any item other than rootItem. How will this affect encoding function if title folder is removed?
* Change symlinks to use relative paths

## HELPFUL TIPS

AA files contain author instead of artist, codec instead of audioFormat and do not contain a duration.

If you play the output file with vlc then it will automatically search for a file named cover.jpg in the same folder and use that as the album cover.

You can safely rename the author and narrator folders locally on your hard drive after creating them with draax. DO NOT rename the title folders. Those folders are targets for the symlinks in the author/narrator folders. If you rename the title folders then the symlinks will no longer function.
