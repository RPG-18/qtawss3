set(SRC Signature.cpp
        Bucket.cpp
        ObjectInfo.cpp
        BucketResponse.cpp
        ServiceResponse.cpp
        Auth.cpp
        Endpoint.cpp
        Error.cpp
)

set(HEADERS Signature.h
            Bucket.h
            Endpoint.h
            ObjectInfo.h
            BucketResponse.h
            ServiceResponse.h
            Auth.h
            Error.h
)

set(LIBS Qt5::Core
         Qt5::Network
)

add_library(core STATIC ${SRC} ${HEADERS})
target_link_libraries(core ${LIBS})