#pragma once

// Version route
#define SERVER_VERSION_ROUTE "/v1"

// Vehicle route
#define SERVER_VEHICLE_ROUTE SERVER_VERSION_ROUTE "/vehicle"

// Vehicle router
#define LOG_ROUTE SERVER_VEHICLE_ROUTE "/log"
#define RFIDS_ROUTE SERVER_VEHICLE_ROUTE "/rfids"
#define OTHER_ROUTE SERVER_VEHICLE_ROUTE "/other"
#define FIRMWARE_ROUTE SERVER_VEHICLE_ROUTE "/firmware"
#define GPS_TRACKING_ROUTE SERVER_VEHICLE_ROUTE "/gps-tracking"

// Vehicle/Log router
#define LOG_FILE_UPLOAD_ENDPOINT LOG_ROUTE "/upload"

// Vehicle/RFIDs router
#define REMOTE_RFID_MD5_CHECKSUM_PATH RFIDS_ROUTE "/md5-checksum"
#define REMOTE_RFID_PATH RFIDS_ROUTE "/"

// Vehicle/Other router
#define CONNECTION_SPEED_TEST_ENDPOINT OTHER_ROUTE "/up-down-test"

// Vehicle/Firmware router
#define LATEST_FIRMWARE_ENDPOINT FIRMWARE_ROUTE "/latest"
#define LATEST_FIRMWARE_DOWNLOAD_PATH LATEST_FIRMWARE_ENDPOINT
#define LATEST_FIRMWARE_SIZE_ENDPOINT LATEST_FIRMWARE_ENDPOINT "/size"

// Vehicle/GPS Tracking router
#define GPS_FILE_UPLOAD_ENDPOINT GPS_TRACKING_ROUTE "/log/upload"
#define REMOTE_GPS_TRACKING_CONSENTED_RFIDS_PATH GPS_TRACKING_ROUTE "/rfids/download"
