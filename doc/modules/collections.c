Note the difference between:  collection resource and collection payload

Resource: the collection links are the child resources


ocpayload.c:

OCRepPayload** OC_CALL OCLinksPayloadArrayCreate(const char* resourceUri,
						 OCEntityHandlerRequest *ehRequest,
						 bool insertSelfLink,
						 size_t* createdArraySize)


occollection.c:

// BuildCollectionLinksPayloadArray
// FIXME: misnamed. builds an array of payloads for child resources of
// resource at uri. the array is not yet a collection - it is not the
// value of a "links" property.  for that you have to take a further step, e.g.:
// OCRepPayload* collectionPayload = OCRepPayloadCreate();
// OCRepPayloadSetPropObjectArrayAsOwner(collectionPayload, "links", linksRepPayloadArray, dim);

// the spec calls this "array of links", but beware, "link" is not a
// resource type.  it's just a set of properties: href, rt, if,
// etc. in particular "rel", "drel", and "anchor".  The term
// "reference" is used too - references are "specified as links" to
// other resources.

// NB: the collection property name "links" is only recommended by the spec!

// find the resource for the uri, convert child resources to an array
// of payloads, for use as a oic.r.links payload
// what about children of the children?
OCRepPayload** BuildCollectionLinksPayloadArray(const char* resourceUri,
						bool isOCFContentFormat,
						OCDevAddr* devAddr,
						bool insertSelfLink,
						size_t* createdArraySize)
