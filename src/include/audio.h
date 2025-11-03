#include <SDL3/SDL_audio.h>

typedef struct {
	
	int channels; // 1 for mono, 2 for stereo 
	int freq; // audio frequency
	size_t buf_size; // size of buffer in frames
	SDL_AudioFormat fmt; // audio format of each  

}Bx_AudioInfo; 

typedef struct{

	size_t size; // memory allocated to data pointer
	size_t top; // top index of the array (this + 1 gives you the amount of elements in array)
	size_t* data; // pointer to data

}Bx_ChannelGroup;

typedef struct{
	
	// state of channel
	Uint8 initialized; // if yes, channel is playable; if no, channel must be initialized.
	Uint8 is_playback; // if yes, buffer is pointer to audio or NULL; if no, is pointer to buffer
	Uint8 is_paused; // if yes, channel is skipped when mixed
	Uint8 is_looping; // if yes, channel will not be freed once audio is finished
	Uint32 is_free; // if yes, audio can be played in channel
	
	// state of audio being played (or buffer)
	Uint32 len; // amount of data stored in channel
	Uint32 prog; // progress of the channel in playing audio. Is disregarded when channel is not playback.
	Uint8* buf; // pointer to buffer of data
	float volume; // volume of channel (initially 0)
	
	Bx_ChannelGroup* group; // is NULL if is not associated to group
	size_t index; // index of channel
	size_t target; // index of target channel (has to be a valid index when assigned)
} Bx_Channel;

typedef struct{
	
	Uint8* buf;
	Uint32 len;
	SDL_AudioSpec* fmt; 

}Bx_Audio;

typedef struct{
	
	// channels data
	Bx_Channel* channels; // pointer to all channels (0 is master)
	size_t channel_count; // number of channels 
	
	// device data
	SDL_AudioDeviceID id;
	SDL_AudioStream* audio_stream;
	Bx_AudioInfo* fmt;

}Bx_Mixer;


// If channel is -1, all channels will be mixed.
int Bx_MixChannels(Bx_Mixer *this, size_t channel, Uint32 data){
	
	if(channel >= this->channel_count){
		fprintf(stderr, "Index_Error: channel index out of bounds.\n");
		return -1;
	}

	return 0;
}

SDL_AudioStreamCallback Bx_CallbackFunc(Bx_Mixer *this, SDL_AudioStream *stream, size_t additional_amount, size_t total_amount){
	
	Bx_MixChannels(this, -1, additional_amount);
	// add extra line to mix between master channel and this stream
	return;
	
}

int Bx_ReallocChannels(Bx_Mixer *this, size_t channel_count, int copy){

	if(channel_count == 0){
		this->channels = NULL;
		this->channel_count = 0;
		return 0;
	}

	if(!copy){
		free(this->channels);
	}
	
	Bx_Channel *chs = malloc(sizeof(Bx_Channel) * channel_count);
	if(chs == NULL){
		fprintf(stderr, "Memory_Error: not enough memory for amount of channels specified.");
		fprintf(stderr, "Please create a smaller amount of channels after object is initialized.\n");
		return -1;
	}

	int lb = 0;
	
	if(copy){
		lb = channel_count < this->channel_count ? channel_count : this->channel_count;
		memcpy(chs, this->channels, sizeof(Bx_Channel) * lb);
		free(this->channels);
	}

	this->channels = chs;
	this->channel_count = channel_count;

	// indicates all created channels as uninitialized
	for(size_t i = lb; i < channel_count; ++i){
		this->channels[i].initialized = 0;
	}

	return 0;
}

Bx_Mixer* Bx_InitializeMixer(Bx_AudioInfo* info, size_t channel_count){

	if(!SDL_Init(SDL_INIT_AUDIO)){
		fprintf(stderr, "SDL_Error: Could not initialize SDL_Audio.\n%s", SDL_GetError()); 
	}

	Bx_Mixer *ret = malloc(sizeof(Bx_Mixer));
	if(ret == NULL){
		fprintf(stderr, "Memory_Error: Could not create mixer object.\n");
		return NULL;
	}

	SDL_AudioSpec sp;
	SDL_AudioSpec *p = &sp;

	if(info != NULL){
		sp = (SDL_AudioSpec) {
			.freq = info->freq,
			.channels = info->channels,
			.format = SDL_AUDIO_S16,
		};
	}
	else p = NULL;

	ret->id = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, p); 
	if(ret->id == 0){
		fprintf(stderr, "SDL_Error: Could not open audio device with provided parameters.\n%s", SDL_GetError());
		return NULL;
	}


	if(info == NULL){
		info = (Bx_AudioInfo*) malloc(sizeof(Bx_AudioInfo));
	}

	if(info == NULL){
		fprintf(stderr, "Memory_Error: Could not create audio format object.\n");
		return NULL;
	}
	
	SDL_GetAudioDeviceFormat(ret->id, &sp, &info->buf_size);
	info->freq = sp.freq;
	info->channels = sp.channels;
	info->fmt = sp.format;
	info->buf_size *= SDL_AUDIO_FRAMESIZE(sp);
	ret->fmt = info;
	ret->channels = NULL;
	ret->audio_stream = SDL_OpenAudioDeviceStream(ret->id, &sp, Bx_CallbackFunc, ret);
	Bx_ReallocChannels(ret, channel_count, 0);
	return ret;
}

int Bx_InitializeChannel(Bx_Mixer* this, size_t index, size_t target, Uint32 is_playback){
	
	if(target > index){
		fprintf(stderr, "Warning: target channels are usually mixed after their source channels;");
		fprintf(stderr, "Please reinit your channel with target as an index lower than that of your channel"); 
		fprintf(stderr, "to have this effect. Channel will still be initialized with this target index.\n");
	}

	if(target >= this->channel_count){
		fprintf(stderr, "Index_Error: selected target channel index is out of bounds");
		return -1;
	}

	if(index >= this->channel_count){
		fprintf(stderr, "Index_Error: selected channel index is out of bounds");
		return -1;
	}
	
	Bx_Channel *channel = this->channels + index;

	channel->volume = 0.f;
	channel->index = index;
	channel->target = target;
	channel->is_playback = is_playback; 

	if(!is_playback){
		
		channel->buf = (Uint8*) malloc(this->fmt->buf_size);
		if(channel->buf == NULL){
			fprintf(stderr, "Memory_Error: Could not initialize buffer for this channel.\n");
			return -1;
		}
		channel->len = this->fmt->buf_size;
	}
	else{

		channel->len = 0;
		channel->buf = NULL;
	}

	channel->initialized = 1;
	return 0;
}

int Bx_ReallocChannelGroup(Bx_ChannelGroup *this, size_t cnt, int copy){
	
	if(cnt == 0){
		free(this->data);
		this->data = NULL;
		this->size = 0;
		this->top = 0;
	}

	if(!copy){
		free(this->data);
	}

	size_t *arr = (size_t*) malloc(sizeof(size_t) * cnt);
	
	if(arr == NULL){
		fprintf(stderr, "Memory_Error: Could not allocate sufficient memory for new stack size.\n");
		return -1;
	}

	if(copy && size){
		lb = this->size < cnt ? this->size : cnt;
		memcpy(arr, this->data, sizeof(size_t) * lb);
		free(this->data);
	}

	this->data = arr;
	this->size = cnt;
	this->top = -1;

	return 0;
}

void Bx_DestroyChannelgroup(Bx_ChannelGroup* this){
	
	if(this == NULL) return;

	free(this->data);
	free(this);
	return;
}

Bx_ChannelGroup* Bx_CreateChannelGroup(Bx_Mixer* this, size_t *channels, size_t len){

	Bx_ChannelGroup *channel = (Bx_ChannelGroup*) malloc(sizeof(Bx_ChannelGroup));
	if(channel == NULL){
		fprintf(stderr, "Memory_Error: insufficient memory for additional object.\n");
		return NULL;
	}
	
	for(int i = 0; i < len; ++i){
		
		if(channels[i] >= this->channel_count){
			fprintf(stderr, "Index_Error: atleast one of the elements on your list are out of bounds.\n");
			Bx_DestroyChannelGroup(channel);
			return NULL;
		}

		channel->top++;
		channel->data[channel->top] = channels[i];
	}

	return 

}

// I give up trying to figure out how to store audio
// The user can do whatever the fuck they want
Bx_Audio* Bx_OpenAudioFile(char* path){
	
	Bx_Audio* file = (Bx_Audio*) malloc(sizeof(Bx_Audio));
	file->fmt = (SDL_AudioSpec*) malloc(sizeof(SDL_AudioSpec));
	SDL_LoadWAV(path, file->fmt, &file->buffer, &file->len);

	return file;

}

void Bx_CloseAudioFile(Bx_Audio* this){
	
	if(this == NULL) return;
	
	free(this->data);
	free(this->fmt);
	free(this);

	return;

}

void Bx_PlayAudio(Bx_Mixer* this, Bx_Audio* clip, size_t index);

void Bx_PauseChannel();
void Bx_ClearChannel();
