
#include "Scanner-Worker.hpp"

using ::v8::Array;
using ::v8::Number;
using ::v8::Value;


void OnigScannerWorker::Execute (){

	bestResult = searcher
		-> Search(source,charOffset);
}


void OnigScannerWorker::HandleOKCallback (){

  Nan::HandleScope scope;

	if( bestResult == NULL ){

		Local<Value> argv [] =
			{ Nan::Null() , Nan::Null() };

		callback -> Call(2,argv,async_resource);

		return;
	}


	auto result = Nan
		::New<Object>();

	Nan::Set( result ,
		Nan::New<String>("index").ToLocalChecked() ,
		Nan::New<Number>(bestResult -> Index()) );

	int resultCount = bestResult
		-> Count();

	auto captures = Nan
		::New<Array>(resultCount);

	for ( int index = 0 ; index < resultCount ; index++ ){

		int captureStart = source
				-> ConvertUtf8OffsetToUtf16(bestResult -> LocationAt(index)) ,

			captureEnd = source
				-> ConvertUtf8OffsetToUtf16(bestResult -> LocationAt(index) + bestResult -> LengthAt(index));

		Local<Object> capture = Nan::New<Object>();

		Nan::Set( capture ,
			Nan::New<String>("index").ToLocalChecked() ,
			Nan::New<Number>(index) );

		Nan::Set( capture ,
			Nan::New<String>("start").ToLocalChecked() ,
			Nan::New<Number>(captureStart) );

		Nan::Set( capture ,
			Nan::New<String>("end").ToLocalChecked() ,
			Nan::New<Number>(captureEnd) );

		Nan::Set( capture ,
			Nan::New<String>("length").ToLocalChecked(),
			Nan::New<Number>(captureEnd - captureStart) );

		Nan::Set(captures, index, capture);
	}


	const auto indices = Nan
		::New<String>("captureIndices")
		. ToLocalChecked();

	Nan::Set(result,indices,captures);


	Local<Value> argv [] =
		{ Nan::Null() , result };

	callback -> Call(2,argv,async_resource);
}
