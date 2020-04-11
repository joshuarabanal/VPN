
namespace TCP{
	struct Option{
		unsigned int type:8;
		unsigned int length:8;//total length 2+ data.length
		unsigned char data [0xff]; 
	};
	namespace OPTIONS{
		enum types{
			End = 0, Empty = 1, MaxSegmentSize = 2,
			WindowScale = 3, SelectiveAcknowlegementPermitted = 4, SelectiveAcknowlegement = 5,
			TImeStamp = 8,
		};
	}
}

namespace TCP::OPTIONS{
	
}
