AcousticBrainz Client
=====================

This client lets you submit your own audio features to the AcousticBrainz project.
For more information about AcousticBrainz, see http://acousticbrainz.org

Getting a feature extractor
---------------------------
To contribute data to AcousticBrainz you first need an _extractor_. This is a
program that analyses your music and generates a file containing data about each track.

We provide static builds of extractors for popular platforms. Find one
for your platform here:

http://acousticbrainz.org/download

Put the extractor and any other included files in the same directory as the submitter client.
It should be called `streaming_extractor_music`.

To run
------

Run the `acousticbrainz-gui` program. Select folders that contain music tagged
with MusicBrainz identifiers. Click Analyze.

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

License
-------
This application is Copyright Music Technology Group, Universitat Pompeu Fabra.
It is available under the terms of the GNU GPL v2. See COPYING for details.

The AcousticBrainz Client is based on the Acoustid fingerprinter by Lukáš Lalinský
available at https://bitbucket.org/acoustid/acoustid-fingerprinter
