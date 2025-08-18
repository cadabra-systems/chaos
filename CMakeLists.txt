cmake_minimum_required(VERSION 3.12.4)

set(COPYRIGHT "Cadabra Systems LLC")
project(
	Chaos
	VERSION 0.25.0.0
	DESCRIPTION "Cadabra C++ Library"
	HOMEPAGE_URL "https://chaos.cadabra.systems"
	LANGUAGES CXX
)

list(APPEND CADABRA_CHAOS_CPP
	${CMAKE_CURRENT_LIST_DIR}/AbstractDeserializer.cpp
	${CMAKE_CURRENT_LIST_DIR}/AbstractSerializer.cpp
	${CMAKE_CURRENT_LIST_DIR}/Any.cpp
	${CMAKE_CURRENT_LIST_DIR}/Base64.cpp
	${CMAKE_CURRENT_LIST_DIR}/CDO/AbstractField.cpp
	${CMAKE_CURRENT_LIST_DIR}/CDO/Field/IntegerField.cpp
	${CMAKE_CURRENT_LIST_DIR}/CDO/FieldSet.cpp
	${CMAKE_CURRENT_LIST_DIR}/CDO/Table.cpp
	${CMAKE_CURRENT_LIST_DIR}/CMF/Deserializer.cpp
	${CMAKE_CURRENT_LIST_DIR}/CMF/Exception.cpp
	${CMAKE_CURRENT_LIST_DIR}/CMF/Serializer.cpp
	${CMAKE_CURRENT_LIST_DIR}/CWS/CNSClient.cpp
	${CMAKE_CURRENT_LIST_DIR}/CWS/FiscusClient.cpp
	${CMAKE_CURRENT_LIST_DIR}/CXML/AbstractAttribute.cpp
	${CMAKE_CURRENT_LIST_DIR}/CXML/AbstractNode.cpp
	${CMAKE_CURRENT_LIST_DIR}/CXML/Attribute/BoolAttribute.cpp
	${CMAKE_CURRENT_LIST_DIR}/CXML/Attribute/DimensionAttribute.cpp
	${CMAKE_CURRENT_LIST_DIR}/CXML/Attribute/DoubleAttribute.cpp
	${CMAKE_CURRENT_LIST_DIR}/CXML/Attribute/IndexAttribute.cpp
	${CMAKE_CURRENT_LIST_DIR}/CXML/Attribute/IntegerAttribute.cpp
	${CMAKE_CURRENT_LIST_DIR}/CXML/Attribute/StringAttribute.cpp
	${CMAKE_CURRENT_LIST_DIR}/CXML/Attribute/UIDAttribute.cpp
	${CMAKE_CURRENT_LIST_DIR}/CXML/Node/BooleanNode.cpp
	${CMAKE_CURRENT_LIST_DIR}/CXML/Node/Document.cpp
	${CMAKE_CURRENT_LIST_DIR}/CXML/Node/Element.cpp
	${CMAKE_CURRENT_LIST_DIR}/CXML/Node/Integer32Node.cpp
	${CMAKE_CURRENT_LIST_DIR}/CXML/Node/Integer64Node.cpp
	${CMAKE_CURRENT_LIST_DIR}/CXML/Node/StringNode.cpp
	${CMAKE_CURRENT_LIST_DIR}/ChatEndpoint.cpp
	${CMAKE_CURRENT_LIST_DIR}/ChatMessage.cpp
	${CMAKE_CURRENT_LIST_DIR}/Color.cpp
	${CMAKE_CURRENT_LIST_DIR}/Comparator.cpp
	${CMAKE_CURRENT_LIST_DIR}/Date.cpp
	${CMAKE_CURRENT_LIST_DIR}/DeviceToken.cpp
	${CMAKE_CURRENT_LIST_DIR}/DummyThread.cpp
	${CMAKE_CURRENT_LIST_DIR}/EmailMessage.cpp
	${CMAKE_CURRENT_LIST_DIR}/Filepath.cpp
	${CMAKE_CURRENT_LIST_DIR}/Heisenbug/BreakingBad.cpp
	${CMAKE_CURRENT_LIST_DIR}/Heisenbug/Flex.hpp
	${CMAKE_CURRENT_LIST_DIR}/IPAddress.cpp
	${CMAKE_CURRENT_LIST_DIR}/ImageMagick.cpp
	${CMAKE_CURRENT_LIST_DIR}/Locale.cpp
	${CMAKE_CURRENT_LIST_DIR}/Log.hpp
	${CMAKE_CURRENT_LIST_DIR}/LogStream.cpp
	${CMAKE_CURRENT_LIST_DIR}/MIME.cpp
	${CMAKE_CURRENT_LIST_DIR}/Money.cpp
	${CMAKE_CURRENT_LIST_DIR}/Memento.cpp
	${CMAKE_CURRENT_LIST_DIR}/NotificationMessage.cpp
	${CMAKE_CURRENT_LIST_DIR}/ODBC/Connection.cpp
	${CMAKE_CURRENT_LIST_DIR}/ODBC/Cursor.cpp
	${CMAKE_CURRENT_LIST_DIR}/ODBC/Error.cpp
	${CMAKE_CURRENT_LIST_DIR}/ODBC/Error/ConnectionError.cpp
	${CMAKE_CURRENT_LIST_DIR}/ODBC/Error/EnvironmentError.cpp
	${CMAKE_CURRENT_LIST_DIR}/ODBC/Error/StatementError.cpp
	${CMAKE_CURRENT_LIST_DIR}/ODBC/Statement.cpp
	${CMAKE_CURRENT_LIST_DIR}/OID.cpp
	${CMAKE_CURRENT_LIST_DIR}/PIDFile.cpp
	${CMAKE_CURRENT_LIST_DIR}/PushMessage.cpp
	${CMAKE_CURRENT_LIST_DIR}/Redis/Command.cpp
	${CMAKE_CURRENT_LIST_DIR}/Redis/Command/AuthCommand.cpp
	${CMAKE_CURRENT_LIST_DIR}/Redis/Command/DelCommand.cpp
	${CMAKE_CURRENT_LIST_DIR}/Redis/Command/ExistsCommand.cpp
	${CMAKE_CURRENT_LIST_DIR}/Redis/Command/ExpireAtCommand.cpp
	${CMAKE_CURRENT_LIST_DIR}/Redis/Command/ExpireCommand.cpp
	${CMAKE_CURRENT_LIST_DIR}/Redis/Command/GetCommand.cpp
	${CMAKE_CURRENT_LIST_DIR}/Redis/Command/HMGetCommand.cpp
	${CMAKE_CURRENT_LIST_DIR}/Redis/Command/HMSetCommand.cpp
	${CMAKE_CURRENT_LIST_DIR}/Redis/Command/PopCommand.cpp
	${CMAKE_CURRENT_LIST_DIR}/Redis/Command/PushCommand.cpp
	${CMAKE_CURRENT_LIST_DIR}/Redis/Command/PushXCommand.cpp
	${CMAKE_CURRENT_LIST_DIR}/Redis/Command/SAddCommand.cpp
	${CMAKE_CURRENT_LIST_DIR}/Redis/Command/SIsMemberCommand.cpp
	${CMAKE_CURRENT_LIST_DIR}/Redis/Command/SRemCommand.cpp
	${CMAKE_CURRENT_LIST_DIR}/Redis/Command/SScanCommand.cpp
	${CMAKE_CURRENT_LIST_DIR}/Redis/Command/SetCommand.cpp
	${CMAKE_CURRENT_LIST_DIR}/Redis/Command/TouchCommand.cpp
	${CMAKE_CURRENT_LIST_DIR}/Redis/CommandLine.cpp
	${CMAKE_CURRENT_LIST_DIR}/Redis/Connection.cpp
	${CMAKE_CURRENT_LIST_DIR}/Redis/Reply.cpp
	${CMAKE_CURRENT_LIST_DIR}/SHA1.cpp
	${CMAKE_CURRENT_LIST_DIR}/SMTPClient.cpp
	${CMAKE_CURRENT_LIST_DIR}/SerializerException.cpp
	${CMAKE_CURRENT_LIST_DIR}/ShortMessage.cpp
	${CMAKE_CURRENT_LIST_DIR}/SpatialObject.cpp
	${CMAKE_CURRENT_LIST_DIR}/String.cpp
	${CMAKE_CURRENT_LIST_DIR}/StringList.cpp
	${CMAKE_CURRENT_LIST_DIR}/Timezone.cpp
	${CMAKE_CURRENT_LIST_DIR}/Timedelta.cpp
	${CMAKE_CURRENT_LIST_DIR}/UID.cpp
	${CMAKE_CURRENT_LIST_DIR}/HTTPClient.cpp
	${CMAKE_CURRENT_LIST_DIR}/WorkerThread.cpp
	${CMAKE_CURRENT_LIST_DIR}/Backtrace.cpp
	${CMAKE_CURRENT_LIST_DIR}/Globus/Globus.cpp
	${CMAKE_CURRENT_LIST_DIR}/Globus/L10N.cpp
	${CMAKE_CURRENT_LIST_DIR}/Globus/T9N.cpp
	${CMAKE_CURRENT_LIST_DIR}/Globus/String.cpp
	${CMAKE_CURRENT_LIST_DIR}/Globus/Character.cpp
)

list(APPEND CADABRA_CHAOS_HPP
	${CMAKE_CURRENT_LIST_DIR}/AbstractDeserializer.hpp
	${CMAKE_CURRENT_LIST_DIR}/AbstractSerializer.hpp
	${CMAKE_CURRENT_LIST_DIR}/AllConvertible.hpp
	${CMAKE_CURRENT_LIST_DIR}/AllTrue.hpp
	${CMAKE_CURRENT_LIST_DIR}/Any.hpp
	${CMAKE_CURRENT_LIST_DIR}/AsyncThen.hpp
	${CMAKE_CURRENT_LIST_DIR}/Atomic.hpp
	${CMAKE_CURRENT_LIST_DIR}/AtomicForwardList.hpp
	${CMAKE_CURRENT_LIST_DIR}/AtomicHashTable.hpp
	${CMAKE_CURRENT_LIST_DIR}/AtomicMap.hpp
	${CMAKE_CURRENT_LIST_DIR}/AtomicPriorityQueue.hpp
	${CMAKE_CURRENT_LIST_DIR}/AtomicQueue.hpp
	${CMAKE_CURRENT_LIST_DIR}/Backtrace.hpp
	${CMAKE_CURRENT_LIST_DIR}/Base64.hpp
	${CMAKE_CURRENT_LIST_DIR}/BiMap.hpp
	${CMAKE_CURRENT_LIST_DIR}/BlockingQueue.hpp
	${CMAKE_CURRENT_LIST_DIR}/CDO/AbstractField.hpp
	${CMAKE_CURRENT_LIST_DIR}/CDO/CDO.hpp
	${CMAKE_CURRENT_LIST_DIR}/CDO/Field/IntegerField.hpp
	${CMAKE_CURRENT_LIST_DIR}/CDO/FieldSet.hpp
	${CMAKE_CURRENT_LIST_DIR}/CDO/Log.hpp
	${CMAKE_CURRENT_LIST_DIR}/CDO/Table.hpp
	${CMAKE_CURRENT_LIST_DIR}/CMF/Deserializer.hpp
	${CMAKE_CURRENT_LIST_DIR}/CMF/Exception.hpp
	${CMAKE_CURRENT_LIST_DIR}/CMF/Proto.hpp
	${CMAKE_CURRENT_LIST_DIR}/CMF/Serializer.hpp
	${CMAKE_CURRENT_LIST_DIR}/CRP.hpp
	${CMAKE_CURRENT_LIST_DIR}/CWS/CC.hpp
	${CMAKE_CURRENT_LIST_DIR}/CWS/CNSClient.hpp
	${CMAKE_CURRENT_LIST_DIR}/CWS/FiscusClient.hpp
	${CMAKE_CURRENT_LIST_DIR}/CWS/Log.hpp
	${CMAKE_CURRENT_LIST_DIR}/CXML/AbstractAttribute.hpp
	${CMAKE_CURRENT_LIST_DIR}/CXML/AbstractNode.hpp
	${CMAKE_CURRENT_LIST_DIR}/CXML/Attribute/BoolAttribute.hpp
	${CMAKE_CURRENT_LIST_DIR}/CXML/Attribute/DimensionAttribute.hpp
	${CMAKE_CURRENT_LIST_DIR}/CXML/Attribute/DoubleAttribute.hpp
	${CMAKE_CURRENT_LIST_DIR}/CXML/Attribute/IndexAttribute.hpp
	${CMAKE_CURRENT_LIST_DIR}/CXML/Attribute/IntegerAttribute.hpp
	${CMAKE_CURRENT_LIST_DIR}/CXML/Attribute/StringAttribute.hpp
	${CMAKE_CURRENT_LIST_DIR}/CXML/Attribute/UIDAttribute.hpp
	${CMAKE_CURRENT_LIST_DIR}/CXML/Node/BooleanNode.hpp
	${CMAKE_CURRENT_LIST_DIR}/CXML/Node/Document.hpp
	${CMAKE_CURRENT_LIST_DIR}/CXML/Node/Element.hpp
	${CMAKE_CURRENT_LIST_DIR}/CXML/Node/Integer32Node.hpp
	${CMAKE_CURRENT_LIST_DIR}/CXML/Node/Integer64Node.hpp
	${CMAKE_CURRENT_LIST_DIR}/CXML/Node/StringNode.hpp
	${CMAKE_CURRENT_LIST_DIR}/ChatEndpoint.hpp
	${CMAKE_CURRENT_LIST_DIR}/ChatMessage.hpp
	${CMAKE_CURRENT_LIST_DIR}/Color.hpp
	${CMAKE_CURRENT_LIST_DIR}/Comparator.hpp
	${CMAKE_CURRENT_LIST_DIR}/Coroutine.hpp
	${CMAKE_CURRENT_LIST_DIR}/Date.hpp
	${CMAKE_CURRENT_LIST_DIR}/Memento.hpp
	${CMAKE_CURRENT_LIST_DIR}/DefectBiMap.hpp
	${CMAKE_CURRENT_LIST_DIR}/Delegate.hpp
	${CMAKE_CURRENT_LIST_DIR}/Demangler.hpp
	${CMAKE_CURRENT_LIST_DIR}/DeviceToken.hpp
	${CMAKE_CURRENT_LIST_DIR}/Dimension.hpp
	${CMAKE_CURRENT_LIST_DIR}/DummyThread.hpp
	${CMAKE_CURRENT_LIST_DIR}/EmailAddress.hpp
	${CMAKE_CURRENT_LIST_DIR}/EmailMessage.hpp
	${CMAKE_CURRENT_LIST_DIR}/Endian.hpp
	${CMAKE_CURRENT_LIST_DIR}/EnumMask.hpp
	${CMAKE_CURRENT_LIST_DIR}/EnumMasquerade.hpp
	${CMAKE_CURRENT_LIST_DIR}/Exception.hpp
	${CMAKE_CURRENT_LIST_DIR}/ExceptionProvoker.hpp
	${CMAKE_CURRENT_LIST_DIR}/Filepath.hpp
	${CMAKE_CURRENT_LIST_DIR}/Filesystem.hpp
	${CMAKE_CURRENT_LIST_DIR}/Flex.hpp
	${CMAKE_CURRENT_LIST_DIR}/Flex/Any.hpp
	${CMAKE_CURRENT_LIST_DIR}/Flex/ODBC.hpp
	${CMAKE_CURRENT_LIST_DIR}/Flex/CTime.hpp
	${CMAKE_CURRENT_LIST_DIR}/Flex/Date.hpp
	${CMAKE_CURRENT_LIST_DIR}/Flex/MIME.hpp
	${CMAKE_CURRENT_LIST_DIR}/Flex/TimePoint.hpp
	${CMAKE_CURRENT_LIST_DIR}/Flex/UID.hpp
	${CMAKE_CURRENT_LIST_DIR}/Flex/UniquePointer.hpp
	${CMAKE_CURRENT_LIST_DIR}/Flex/URI.hpp
	${CMAKE_CURRENT_LIST_DIR}/Flex/IPAddress.hpp
	${CMAKE_CURRENT_LIST_DIR}/Flex/Variant.hpp
	${CMAKE_CURRENT_LIST_DIR}/Function.hpp
	${CMAKE_CURRENT_LIST_DIR}/Gender.hpp
	${CMAKE_CURRENT_LIST_DIR}/Hash.hpp
	${CMAKE_CURRENT_LIST_DIR}/HazardPointer.hpp
	${CMAKE_CURRENT_LIST_DIR}/Heisenbug.hpp
	${CMAKE_CURRENT_LIST_DIR}/Heisenbug/AtomicForwardList.hpp
	${CMAKE_CURRENT_LIST_DIR}/Heisenbug/AtomicHashTable.hpp
	${CMAKE_CURRENT_LIST_DIR}/Heisenbug/BreakingBad.hpp
	${CMAKE_CURRENT_LIST_DIR}/Heisenbug/CDO.hpp
	${CMAKE_CURRENT_LIST_DIR}/Heisenbug/Globus.hpp
	${CMAKE_CURRENT_LIST_DIR}/Heisenbug/Date.hpp
	${CMAKE_CURRENT_LIST_DIR}/Heisenbug/Any.hpp
	${CMAKE_CURRENT_LIST_DIR}/Heisenbug/System.hpp
	${CMAKE_CURRENT_LIST_DIR}/Heisenbug/Basic.hpp
	${CMAKE_CURRENT_LIST_DIR}/IPAddress.hpp
	${CMAKE_CURRENT_LIST_DIR}/ImageMagick.hpp
	${CMAKE_CURRENT_LIST_DIR}/InputEcho.hpp
	${CMAKE_CURRENT_LIST_DIR}/IsDetected.hpp
	${CMAKE_CURRENT_LIST_DIR}/Listener.hpp
	${CMAKE_CURRENT_LIST_DIR}/Locale.hpp
	${CMAKE_CURRENT_LIST_DIR}/Log.hpp
	${CMAKE_CURRENT_LIST_DIR}/LogFormatLevel.hpp
	${CMAKE_CURRENT_LIST_DIR}/LogRegister.hpp
	${CMAKE_CURRENT_LIST_DIR}/LogStream.hpp
	${CMAKE_CURRENT_LIST_DIR}/MD5.hpp
	${CMAKE_CURRENT_LIST_DIR}/MIME.hpp
	${CMAKE_CURRENT_LIST_DIR}/MakeUnique.hpp
	${CMAKE_CURRENT_LIST_DIR}/Map.hpp
	${CMAKE_CURRENT_LIST_DIR}/MarkedPointer.hpp
	${CMAKE_CURRENT_LIST_DIR}/Money.hpp
	${CMAKE_CURRENT_LIST_DIR}/MongoDB/Log.hpp
	${CMAKE_CURRENT_LIST_DIR}/MongoDB/MongoDB.hpp
	${CMAKE_CURRENT_LIST_DIR}/NotificationMessage.hpp
	${CMAKE_CURRENT_LIST_DIR}/NullStream.hpp
	${CMAKE_CURRENT_LIST_DIR}/ODBC/Connection.hpp
	${CMAKE_CURRENT_LIST_DIR}/ODBC/Cursor.hpp
	${CMAKE_CURRENT_LIST_DIR}/ODBC/Error.hpp
	${CMAKE_CURRENT_LIST_DIR}/ODBC/Error/ConnectionError.hpp
	${CMAKE_CURRENT_LIST_DIR}/ODBC/Error/EnvironmentError.hpp
	${CMAKE_CURRENT_LIST_DIR}/ODBC/Error/StatementError.hpp
	${CMAKE_CURRENT_LIST_DIR}/ODBC/Exception.hpp
	${CMAKE_CURRENT_LIST_DIR}/ODBC/Log.hpp
	${CMAKE_CURRENT_LIST_DIR}/ODBC/ODBC.hpp
	${CMAKE_CURRENT_LIST_DIR}/ODBC/Statement.hpp
	${CMAKE_CURRENT_LIST_DIR}/OID.hpp
	${CMAKE_CURRENT_LIST_DIR}/ObjectPool.hpp
	${CMAKE_CURRENT_LIST_DIR}/Optional.hpp
	${CMAKE_CURRENT_LIST_DIR}/PIDFile.hpp
	${CMAKE_CURRENT_LIST_DIR}/PhoneNumber.hpp
	${CMAKE_CURRENT_LIST_DIR}/Process.hpp
	${CMAKE_CURRENT_LIST_DIR}/ProgramName.hpp
	${CMAKE_CURRENT_LIST_DIR}/PushMessage.hpp
	${CMAKE_CURRENT_LIST_DIR}/RabbitMQ/Log.hpp
	${CMAKE_CURRENT_LIST_DIR}/RabbitMQ/RabbitMQ.hpp
	${CMAKE_CURRENT_LIST_DIR}/Redis/Command.hpp
	${CMAKE_CURRENT_LIST_DIR}/Redis/Command/AuthCommand.hpp
	${CMAKE_CURRENT_LIST_DIR}/Redis/Command/DelCommand.hpp
	${CMAKE_CURRENT_LIST_DIR}/Redis/Command/ExistsCommand.hpp
	${CMAKE_CURRENT_LIST_DIR}/Redis/Command/ExpireAtCommand.hpp
	${CMAKE_CURRENT_LIST_DIR}/Redis/Command/ExpireCommand.hpp
	${CMAKE_CURRENT_LIST_DIR}/Redis/Command/GetCommand.hpp
	${CMAKE_CURRENT_LIST_DIR}/Redis/Command/HMGetCommand.hpp
	${CMAKE_CURRENT_LIST_DIR}/Redis/Command/HMSetCommand.hpp
	${CMAKE_CURRENT_LIST_DIR}/Redis/Command/PopCommand.hpp
	${CMAKE_CURRENT_LIST_DIR}/Redis/Command/PushCommand.hpp
	${CMAKE_CURRENT_LIST_DIR}/Redis/Command/PushXCommand.hpp
	${CMAKE_CURRENT_LIST_DIR}/Redis/Command/SAddCommand.hpp
	${CMAKE_CURRENT_LIST_DIR}/Redis/Command/SIsMemberCommand.hpp
	${CMAKE_CURRENT_LIST_DIR}/Redis/Command/SRemCommand.hpp
	${CMAKE_CURRENT_LIST_DIR}/Redis/Command/SScanCommand.hpp
	${CMAKE_CURRENT_LIST_DIR}/Redis/Command/SetCommand.hpp
	${CMAKE_CURRENT_LIST_DIR}/Redis/Command/TouchCommand.hpp
	${CMAKE_CURRENT_LIST_DIR}/Redis/CommandLine.hpp
	${CMAKE_CURRENT_LIST_DIR}/Redis/Connection.hpp
	${CMAKE_CURRENT_LIST_DIR}/Redis/Log.hpp
	${CMAKE_CURRENT_LIST_DIR}/Redis/Redis.hpp
	${CMAKE_CURRENT_LIST_DIR}/Redis/Reply.hpp
	${CMAKE_CURRENT_LIST_DIR}/Reflex.hpp
	${CMAKE_CURRENT_LIST_DIR}/Murmur3.hpp
	${CMAKE_CURRENT_LIST_DIR}/SHA1.hpp
	${CMAKE_CURRENT_LIST_DIR}/SHA256.hpp
	${CMAKE_CURRENT_LIST_DIR}/SMTPClient.hpp
	${CMAKE_CURRENT_LIST_DIR}/SafePointer.hpp
	${CMAKE_CURRENT_LIST_DIR}/SerializerException.hpp
	${CMAKE_CURRENT_LIST_DIR}/SharedQueue.hpp
	${CMAKE_CURRENT_LIST_DIR}/ShortMessage.hpp
	${CMAKE_CURRENT_LIST_DIR}/Singleton.hpp
	${CMAKE_CURRENT_LIST_DIR}/SpatialObject.hpp
	${CMAKE_CURRENT_LIST_DIR}/String.hpp
	${CMAKE_CURRENT_LIST_DIR}/StringList.hpp
	${CMAKE_CURRENT_LIST_DIR}/SyslogStream.hpp
	${CMAKE_CURRENT_LIST_DIR}/Task.hpp
	${CMAKE_CURRENT_LIST_DIR}/TaskThread.hpp
	${CMAKE_CURRENT_LIST_DIR}/TemporaryFile.hpp
	${CMAKE_CURRENT_LIST_DIR}/ThreadObject.hpp
	${CMAKE_CURRENT_LIST_DIR}/ThreadPool.hpp
	${CMAKE_CURRENT_LIST_DIR}/Time.hpp
	${CMAKE_CURRENT_LIST_DIR}/Timedelta.hpp
	${CMAKE_CURRENT_LIST_DIR}/Timezone.hpp
	${CMAKE_CURRENT_LIST_DIR}/TriBool.hpp
	${CMAKE_CURRENT_LIST_DIR}/Type.hpp
	${CMAKE_CURRENT_LIST_DIR}/UID.hpp
	${CMAKE_CURRENT_LIST_DIR}/URI.hpp
	${CMAKE_CURRENT_LIST_DIR}/VoidType.hpp
	${CMAKE_CURRENT_LIST_DIR}/HTTPClient.hpp
	${CMAKE_CURRENT_LIST_DIR}/WorkerPool.hpp
	${CMAKE_CURRENT_LIST_DIR}/WorkerThread.hpp
	${CMAKE_CURRENT_LIST_DIR}/Globus/Globus.hpp
	${CMAKE_CURRENT_LIST_DIR}/Globus/L10N.hpp
	${CMAKE_CURRENT_LIST_DIR}/Globus/T9N.hpp
	${CMAKE_CURRENT_LIST_DIR}/Globus/String.hpp
	${CMAKE_CURRENT_LIST_DIR}/Globus/Character.hpp
)

find_package(PkgConfig REQUIRED)

include(Compiler.cmake)
include(Unwind.cmake)
include(ICU.cmake)
include(UUid.cmake)
include(OpenSSL.cmake)
include(HiRedis.cmake)
include(ImageMagick.cmake)
include(Magic.cmake)

find_package(Threads REQUIRED)
find_package(ODBC REQUIRED)
find_package(CURL REQUIRED)

add_compile_definitions($<$<CONFIG:DEBUG>:CADABRA_CHAOS_DEBUG_MODE>)
if (CADABRA_CHAOS_BUILD_LABEL)
	add_compile_definitions(CADABRA_CHAOS_BUILD_LABEL="${CADABRA_CHAOS_BUILD_LABEL}")
endif ()
if (CADABRA_CHAOS_SOURCE_REVISION)
	add_compile_definitions(CADABRA_CHAOS_SOURCE_REVISION="${CADABRA_CHAOS_SOURCE_REVISION}")
endif ()

add_library(CADABRA_CHAOS_STATIC STATIC ${CADABRA_CHAOS_CPP} ${CADABRA_CHAOS_HPP})
set_target_properties(CADABRA_CHAOS_STATIC PROPERTIES OUTPUT_NAME chaos POSITION_INDEPENDENT_CODE ON)
target_link_libraries(CADABRA_CHAOS_STATIC PUBLIC ICU::uc ICU::i18n)
target_link_libraries(CADABRA_CHAOS_STATIC PUBLIC HiRedis::HiRedis)
target_link_libraries(CADABRA_CHAOS_STATIC PUBLIC Threads::Threads)
target_link_libraries(CADABRA_CHAOS_STATIC PUBLIC ${CMAKE_DL_LIBS})
target_link_libraries(CADABRA_CHAOS_STATIC PUBLIC ODBC::ODBC)
target_link_libraries(CADABRA_CHAOS_STATIC PUBLIC OpenSSL::SSL OpenSSL::Crypto)
target_link_libraries(CADABRA_CHAOS_STATIC PUBLIC CURL::libcurl)
target_link_libraries(CADABRA_CHAOS_STATIC PUBLIC ImageMagick::Core ImageMagick::Wand)
target_link_libraries(CADABRA_CHAOS_STATIC PUBLIC libMagic::libMagic)
if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	target_link_libraries(CADABRA_CHAOS_STATIC PUBLIC Unwind::Unwind)
	target_link_libraries(CADABRA_CHAOS_STATIC PUBLIC UUid::UUid)
endif ()
install(TARGETS CADABRA_CHAOS_STATIC RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR})

add_library(CADABRA_CHAOS_SHARED SHARED $<TARGET_OBJECTS:CADABRA_CHAOS_STATIC>)
set_target_properties(CADABRA_CHAOS_SHARED PROPERTIES OUTPUT_NAME chaos POSITION_INDEPENDENT_CODE ON)
target_link_libraries(CADABRA_CHAOS_SHARED PRIVATE CADABRA_CHAOS_STATIC)
install(TARGETS CADABRA_CHAOS_SHARED RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR})

add_executable(CADABRA_CHAOS_HEISENBUG Heisenbug.cpp ${CADABRA_CHAOS_CPP} ${CADABRA_CHAOS_HPP})
set_target_properties(CADABRA_CHAOS_HEISENBUG PROPERTIES OUTPUT_NAME chaos-heisenbug)
target_link_libraries(CADABRA_CHAOS_HEISENBUG PRIVATE CADABRA_CHAOS_STATIC)
if (CHAOS_CXX_SANITIZER_FLAGS)
	target_link_options(CADABRA_CHAOS_HEISENBUG PRIVATE ${CHAOS_CXX_SANITIZER_FLAGS})
endif ()
if (CHAOS_CXX_OPTIMIZER_FLAGS)
	target_compile_options(CADABRA_CHAOS_HEISENBUG PRIVATE ${CHAOS_CXX_OPTIMIZER_FLAGS})
endif ()

install(TARGETS CADABRA_CHAOS_HEISENBUG RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
