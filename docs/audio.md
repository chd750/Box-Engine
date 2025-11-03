# Audio

This is the audio-playing functionality of the Box Engine kernel. All functions and type declarations relating to audio
playback are declared here.

# How to use

To initialize your audio mixer for playback, create a ``Bx_AudioInfo`` object with the proper attributes, and pass it to
``Bx_InitializeMixer()``. You will then get a mixer object initialized (with all formatting info stored in the ``fmt``
attribute. Once your format is initialized, you can now load your audio files and assign them to channels. Your audio
will automatically start playing as soon as it is assigned to a channel. 

Note that channels only play one audio file at a time, so if you plan to play many pieces of audio at the same time, 
allocate a row of channels, and assign your audio to whichever one is free (a stack would be the datastructure of
choice in my opinion, assuming channels don't need to accessed on an indexed basis).

# Reference

Bx_Mixer
Bx_Channel
Bx_AudioInfo
Bx_AudioFile

Bx_InitializeMixer
Bx_DestroyMixer
Bx_LoadAudio <!-- download audio -->
Bx_PlayAudio
Bx_MixChannel
Bx_PauseChannel
Bx_PauseAudio
