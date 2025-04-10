var OpusLib = {
  is_encoder_supported_webcodecs: function() {
    console.log("a");
    return false;
  },

  opus_encoder_init: function(st, Fs, channels, application) {
    this.is_encoder_supported_webcodecs();
  },
};

const hoge = new OpusLib();

hoge.opus_encoder_init();


