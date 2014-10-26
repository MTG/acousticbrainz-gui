AcousticBrainz Client
=====================


For more information about acousticbrainz, see http://acousticbrainz.org

This client lets you submit your own audio features to the acousticbrainz project.

Getting a feature extractor
---------------------------
To contribute data to acousticbrainz you first need an _extractor_. This is a
program that analyses your music and generates a file.

We provide static builds of extractors for popular platforms. Find one
for your platform here:

http://acousticbrainz.org/download

Put the extractor in the directory `extractor`. It should be called `streaming_extractor_music`.
These files require no additional dependencies.

To run
------

Run the `acousticbrainz-gui` program. Select folders that contain music tagged with
Musicbrainz identifiers. Click Analyze.

Dependencies
------------

 * Qt4 <http://www.qt.io/>
 * QJson <http://qjson.sourceforge.net/>

Installation
------------

```
    $ cmake -DCMAKE_BUILD_TYPE=Release .
    $ make
```
