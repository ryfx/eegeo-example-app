// Copyright eeGeo Ltd (2012-2015), All Rights Reserved

#include "document.h"
#include "ApplicationConfigurationJsonParser.h"
#include "MathFunc.h"
#include "ConfigSections.h"
#include "StringHelpers.h"

namespace ExampleApp
{
    namespace ApplicationConfig
    {
        namespace SdkModel
        {
            namespace
            {
                const std::string Name = "name";
                const std::string StartLocationLatitude = "start_location_latitude";
                const std::string StartLocationLongitude = "start_location_longitude";
                const std::string StartLocationAltitude = "start_location_altitude";
                const std::string StartLocationDistance = "start_location_distance";
                const std::string StartLocationOrientationDegrees = "start_location_orientation_degrees";
                const std::string TryStartAtGpsLocation = "try_start_at_gps_location";
                const std::string EegeoApiKey = "eegeo_api_key";
                const std::string CoverageTreeManifestURL = "coveragetree_manifest_url";
                const std::string ThemeManifestURL = "theme_manifest_url";
                const std::string EmbeddedThemeTexturePath = "embedded_theme_texture_path";
                const std::string GoogleAnalyticsReferrerToken = "google_analytics_referrer_token";
                const std::string FlurryAppKey = "flurry_app_key";
                const std::string YelpConsumerKey = "yelp_consumer_key";
                const std::string YelpConsumerSecret = "yelp_consumer_secret";
                const std::string YelpOAuthToken = "yelp_oauth_token";
                const std::string YelpOAuthTokenSecret = "yelp_oauth_token_secret";
                const std::string GeoNamesUserName = "geonames_username";
                const std::string EegeoSearchServiceUrl = "eegeo_search_service_url";
                const std::string MyPinsWebServiceUrl = "mypins_web_service_url";
                const std::string MyPinsWebServiceAuthToken = "mypins_web_service_auth_token";
                const std::string MyPinsPoiSetId = "mypins_poi_set_id";
                const std::string IsKioskTouchInputEnabled = "is_kiosk_touch_input_enabled";
                const std::string IsInKioskMode = "is_in_kiosk_mode";
                const std::string StartAppInFullscreen = "start_app_in_fullscreen";
                const std::string UseJapaneseFont = "use_japanese_font";
                const std::string IndoorTrackedBuildings = "indoor_tracked_buildings";
                const std::string InteriorId = "interior_id";
                const std::string Type = "type";
                const std::string Config = "config";
                const std::string ApiKey = "api_key";
                const std::string ApiSecret = "api_secret";
                const std::string FloorMapping = "floor_mapping";
                const std::string BuildingFloorIndex = "building_floor_index";
                const std::string TrackedFloorIndex = "tracked_floor_index";
                const std::string LocationDegrees = "location_degrees";
                const std::string Latitude = "latitude";
                const std::string Longitude = "longitude";
                const std::string Altitude = "altitude";
                const std::string OrientationDegrees = "orientation_degrees";
                const std::string FixedIndoorLocation = "fixed_indoor_location";
                const std::string AttractModeTargetSpline = "attract_mode_target_spline";
                const std::string AttractModePositionSpline = "attract_mode_position_spline";
                const std::string AttractModeTimeoutMillis = "attract_mode_timeout_millis";
                const std::string AttractModePlaybackSpeed = "attract_mode_playback_speed";
                const std::string OptionsAdminPassword = "options_admin_password";
                const std::string SurveyTimeRequirementSec = "survey_time_requirement_sec";
                const std::string TimerSurveyUrl = "timer_survey_url";
                
                std::string ParseStringOrDefault(rapidjson::Document& document, const std::string& key, const std::string& defaultValue)
                {
                    if (document.HasMember(key.c_str()) && document[key.c_str()].IsString())
                    {
                        const std::string& value = document[key.c_str()].GetString();
                        if (!value.empty())
                        {
                            return value;
                        }
                    }
                    return defaultValue;
                }
                
                bool ParseBoolOrDefault(rapidjson::Document& document, const std::string& key, bool defaultValue)
                {
                    if (document.HasMember(key.c_str()))
                    {
                        if(document[key.c_str()].IsBool())
                        {
                            return document[key.c_str()].GetBool();
                        }
                        else if(document[key.c_str()].IsString())
                        {
                            std::string documentString = document[key.c_str()].GetString();
                            if(documentString == "true")
                            {
                                return true;
                            }
                            else if(documentString == "false")
                            {
                                return false;
                            }
                        }
                    }
                    return defaultValue;
                }
                
                double ParseDoubleOrDefault(rapidjson::Document& document, const std::string& key, double defaultValue)
                {
                    if (document.HasMember(key.c_str()))
                    {
                        if(document[key.c_str()].IsDouble())
                        {
                            return document[key.c_str()].GetDouble();
                        }
                        else if(document[key.c_str()].IsString())
                        {
                            double valueFromString;
                            if(Eegeo::Helpers::TryParseDouble(document[key.c_str()].GetString(), valueFromString))
                            {
                                return valueFromString;
                            }
                        }
                    }
                    return defaultValue;
                }

                int ParseIntOrDefault(rapidjson::Document& document, const std::string& key, int defaultValue)
                {
                    if (document.HasMember(key.c_str()))
                    {
                        if(document[key.c_str()].IsInt())
                        {
                            return document[key.c_str()].GetInt();
                        }
                        else if(document[key.c_str()].IsString())
                        {
                            int valueFromString;
                            if(Eegeo::Helpers::TryParseInt(document[key.c_str()].GetString(), valueFromString))
                            {
                                return valueFromString;
                            }
                        }
                    }
                    return defaultValue;
                }

                Eegeo::Space::LatLong ParseLatLong(const rapidjson::Value& latlong)
                {
                    return Eegeo::Space::LatLong::FromDegrees(
                            latlong.HasMember(Latitude.c_str()) && latlong[Latitude.c_str()].IsDouble() ? latlong[Latitude.c_str()].GetDouble() : 0.0,
                            latlong.HasMember(Longitude.c_str()) && latlong[Longitude.c_str()].IsDouble() ? latlong[Longitude.c_str()].GetDouble() : 0.0);
                }

                Eegeo::Space::LatLongAltitude ParseLatLongAltitude(const rapidjson::Value& location)
                {
                    return Eegeo::Space::LatLongAltitude::FromDegrees(
                            location.HasMember(Latitude.c_str()) && location[Latitude.c_str()].IsDouble() ? location[Latitude.c_str()].GetDouble() : 0.0,
                            location.HasMember(Longitude.c_str()) && location[Longitude.c_str()].IsDouble() ? location[Longitude.c_str()].GetDouble() : 0.0,
                            location.HasMember(Altitude.c_str()) && location[Altitude.c_str()].IsDouble() ? location[Altitude.c_str()].GetDouble() : 0.0);
                }

                template <typename T, typename Parser>
                std::vector<T> ParseArray(const rapidjson::Value& xsAst, Parser parse)
                {
                    std::vector<T> xs;
                    if (xsAst.IsArray())
                    {
                        for (rapidjson::SizeType i = 0; i < xsAst.Size(); ++i)
                        {
                            xs.push_back(parse(xsAst[i]));
                        }
                    }

                    return xs;
                }

                SdkModel::ApplicationFixedIndoorLocation ParseFixedIndoorLocation(const rapidjson::Value& fixedIndoorLocation)
                {
                    const rapidjson::Value empty(rapidjson::kObjectType);
                    return SdkModel::ApplicationFixedIndoorLocation(
                        ParseLatLong(fixedIndoorLocation.HasMember(LocationDegrees.c_str()) ? fixedIndoorLocation[LocationDegrees.c_str()] : empty),
                        fixedIndoorLocation.HasMember(InteriorId.c_str()) && fixedIndoorLocation[InteriorId.c_str()].IsString()
                            ? fixedIndoorLocation[InteriorId.c_str()].GetString()
                            : "",
                        fixedIndoorLocation.HasMember(BuildingFloorIndex.c_str()) && fixedIndoorLocation[BuildingFloorIndex.c_str()].IsInt()
                            ? fixedIndoorLocation[BuildingFloorIndex.c_str()].GetInt()
                            : 0,
                        fixedIndoorLocation.HasMember(OrientationDegrees.c_str()) && fixedIndoorLocation[OrientationDegrees.c_str()].IsDouble()
                            ? fixedIndoorLocation[OrientationDegrees.c_str()].GetDouble()
                            : 180.0);
                }
            }

            ApplicationConfigurationJsonParser::ApplicationConfigurationJsonParser(const ApplicationConfiguration& defaultConfig)
            : m_defaultConfig(defaultConfig)
            {
                
            }
            
            ApplicationConfiguration ApplicationConfigurationJsonParser::ParseConfiguration(const std::string& serialized)
            {
                rapidjson::Document document;
                
                const bool hasParseError(document.Parse<0>(serialized.c_str()).HasParseError());
                Eegeo_ASSERT(!hasParseError, "unable to parse app configuration");

                const double startLocationLatitude = ParseDoubleOrDefault(document, StartLocationLatitude, m_defaultConfig.InterestLocation().GetLatitudeInDegrees());
                const double startLocationLongitude = ParseDoubleOrDefault(document, StartLocationLongitude, m_defaultConfig.InterestLocation().GetLongitudeInDegrees());
                const double startLocationAltitude = ParseDoubleOrDefault(document, StartLocationAltitude, m_defaultConfig.InterestLocation().GetAltitude());
                const double startLocationDistance = ParseDoubleOrDefault(document, StartLocationDistance, m_defaultConfig.DistanceToInterestMetres());
                const double startLocationOrientationDegrees = ParseDoubleOrDefault(document, StartLocationOrientationDegrees, m_defaultConfig.OrientationDegrees());

                const Eegeo::Space::LatLongAltitude& lla = Eegeo::Space::LatLongAltitude::FromDegrees(startLocationLatitude,
                                                                                                   startLocationLongitude,
                                                                                                   startLocationAltitude);
                
                const bool tryStartAtGpsLocation = ParseBoolOrDefault(document, TryStartAtGpsLocation, m_defaultConfig.TryStartAtGpsLocation());

                const std::string& name = ParseStringOrDefault(document, Name, m_defaultConfig.Name());
                const std::string& eegeoApiKey = ParseStringOrDefault(document, EegeoApiKey, m_defaultConfig.EegeoApiKey());
                const std::string& coverageTreeManifestURL = ParseStringOrDefault(document, CoverageTreeManifestURL, m_defaultConfig.CoverageTreeManifestURL());
                const std::string& themeManifestURL = ParseStringOrDefault(document, ThemeManifestURL, m_defaultConfig.ThemeManifestURL());
                const std::string& embeddedThemeTexturePath = ParseStringOrDefault(document, EmbeddedThemeTexturePath, m_defaultConfig.EmbeddedThemeTexturePath());
                const std::string& googleAnalyticsReferrerToken = ParseStringOrDefault(document, GoogleAnalyticsReferrerToken, m_defaultConfig.GoogleAnalyticsReferrerToken());
                const std::string& flurryAppKey = ParseStringOrDefault(document, FlurryAppKey, m_defaultConfig.FlurryAppKey());
                const std::string& yelpConsumerKey = ParseStringOrDefault(document, YelpConsumerKey, m_defaultConfig.YelpConsumerKey());
                const std::string& yelpConsumerSecret = ParseStringOrDefault(document, YelpConsumerSecret, m_defaultConfig.YelpConsumerSecret());
                const std::string& yelpOAuthToken = ParseStringOrDefault(document, YelpOAuthToken, m_defaultConfig.YelpOAuthToken());
                const std::string& yelpOAuthTokenSecret = ParseStringOrDefault(document, YelpOAuthTokenSecret, m_defaultConfig.YelpOAuthTokenSecret());
                const std::string& geoNamesUserName = ParseStringOrDefault(document, GeoNamesUserName, m_defaultConfig.GeoNamesUserName());
                const std::string& eegeoSearchServiceUrl = ParseStringOrDefault(document, EegeoSearchServiceUrl, m_defaultConfig.EegeoSearchServiceUrl());
                const std::string& myPinsWebServiceUrl = ParseStringOrDefault(document, MyPinsWebServiceUrl, m_defaultConfig.MyPinsWebServiceUrl());
                const std::string& myPinsWebServiceAuthToken = ParseStringOrDefault(document, MyPinsWebServiceAuthToken, m_defaultConfig.MyPinsWebServiceAuthToken());
                const std::string& myPinsPoiSetId = ParseStringOrDefault(document, MyPinsPoiSetId, m_defaultConfig.MyPinsPoiSetId());
                bool isKioskTouchInputEnabled = ParseBoolOrDefault(document, IsKioskTouchInputEnabled, m_defaultConfig.IsKioskTouchInputEnabled());
                bool isInKioskMode = ParseBoolOrDefault(document, IsInKioskMode, m_defaultConfig.IsInKioskMode());
                bool startFullscreen = ParseBoolOrDefault(document, StartAppInFullscreen, m_defaultConfig.ShouldStartFullscreen());
                bool useJapaneseFont = ParseBoolOrDefault(document, UseJapaneseFont, m_defaultConfig.UseJapaneseFont());

                std::map<std::string, SdkModel::ApplicationInteriorTrackingInfo> interiorTrackingInfoList;
                if(document.HasMember(IndoorTrackedBuildings.c_str()) && !document[IndoorTrackedBuildings.c_str()].IsNull())
                {
                    const rapidjson::Value& indoorTrackedBuildingsArray = document[IndoorTrackedBuildings.c_str()];
                    ParseIndoorTrackingInfo(interiorTrackingInfoList, indoorTrackedBuildingsArray);
                }

                const rapidjson::Value empty(rapidjson::kObjectType);
                const SdkModel::ApplicationFixedIndoorLocation fixedIndoorLocation(ParseFixedIndoorLocation(document.HasMember(FixedIndoorLocation.c_str()) ? document[FixedIndoorLocation.c_str()] : empty));

                const std::vector<Eegeo::Space::LatLongAltitude> attractModeTargetSplinePoints(
                    ParseArray<Eegeo::Space::LatLongAltitude>(document.HasMember(AttractModeTargetSpline.c_str())
                                                                ? document[AttractModeTargetSpline.c_str()]
                                                                : empty,
                                                              ParseLatLongAltitude));
                const std::vector<Eegeo::Space::LatLongAltitude> attractModePositionSplinePoints(
                    ParseArray<Eegeo::Space::LatLongAltitude>(document.HasMember(AttractModePositionSpline.c_str())
                                                                ? document[AttractModePositionSpline.c_str()]
                                                                : empty,
                                                              ParseLatLongAltitude));

                const long long attractModeTimeoutMillis = ParseIntOrDefault(document, AttractModeTimeoutMillis, static_cast<int>(m_defaultConfig.AttractModeTimeoutMs()));
                const float attractModePlaybackSpeed = ParseDoubleOrDefault(document, AttractModePlaybackSpeed, m_defaultConfig.AttractModePlaybackSpeed());

                const std::string optionsAdminPassword(ParseStringOrDefault(document, OptionsAdminPassword, m_defaultConfig.OptionsAdminPassword()));
                
                const long long surveyTimeRequirementSec = ParseIntOrDefault(document, SurveyTimeRequirementSec, static_cast<int>(m_defaultConfig.SurveyRequirementTimeSec()));
                
                const std::string timerSurveyUrl = ParseStringOrDefault(document, TimerSurveyUrl, m_defaultConfig.TimerSurveyUrl());

                return ApplicationConfiguration(
                    name,
                    eegeoApiKey,
                    m_defaultConfig.ProductVersion(),
                    m_defaultConfig.BuildNumber(),
                    m_defaultConfig.CombinedVersionString(),
                    coverageTreeManifestURL,
                    themeManifestURL,
                    embeddedThemeTexturePath,
                    lla,
                    startLocationDistance,
                    startLocationOrientationDegrees,
                    tryStartAtGpsLocation,
                    startFullscreen,
                    googleAnalyticsReferrerToken,
                    flurryAppKey,
                    yelpConsumerKey,
                    yelpConsumerSecret,
                    yelpOAuthToken,
                    yelpOAuthTokenSecret,
                    geoNamesUserName,
                    eegeoSearchServiceUrl,
                    myPinsWebServiceUrl,
                    myPinsWebServiceAuthToken,
                    myPinsPoiSetId,
                    isKioskTouchInputEnabled,
                    isInKioskMode,
                    useJapaneseFont,
                    interiorTrackingInfoList,
                    serialized,
                    fixedIndoorLocation,
                    attractModeTargetSplinePoints,
                    attractModePositionSplinePoints,
                    attractModeTimeoutMillis,
                    attractModePlaybackSpeed,
                    optionsAdminPassword,
                    surveyTimeRequirementSec,
                    timerSurveyUrl
                );
            }
            
            bool ApplicationConfigurationJsonParser::IsValidConfig(const std::string& serialized)
            {
                rapidjson::Document document;
                const bool hasParseError(document.Parse<0>(serialized.c_str()).HasParseError());
                return !hasParseError;
            }
            
            void ApplicationConfigurationJsonParser::ParseIndoorTrackingInfo(std::map<std::string, SdkModel::ApplicationInteriorTrackingInfo>& interiorTrackingInfoList,
                                                                             const rapidjson::Value& indoorTrackedBuildingsArray)
            {
                for(rapidjson::SizeType i = 0; i < indoorTrackedBuildingsArray.Size(); ++i)
                {
                    const rapidjson::Value& building = indoorTrackedBuildingsArray[i];
                    
                    Eegeo_ASSERT(building.HasMember(InteriorId.c_str()), "Interior Id not found");
                    const std::string& id = building[InteriorId.c_str()].GetString();
                    const Eegeo::Resources::Interiors::InteriorId& interiorId(id);
                    
                    Eegeo_ASSERT(building.HasMember(Type.c_str()), "Type not found");
                    const std::string& type = building[Type.c_str()].GetString();

                    
                    Eegeo_ASSERT(building.HasMember(Config.c_str()), "Config not found");
                    const std::string& apiKey = building[Config.c_str()][0][ApiKey.c_str()].GetString();
                    const std::string& apiSecret = building[Config.c_str()][0][ApiSecret.c_str()].GetString();
                    SdkModel::ApplicationInteriorTrackingConfig interiorTrackingConfig(apiKey, apiSecret);
                    
                    Eegeo_ASSERT(building.HasMember(FloorMapping.c_str()), "FloorMapping not found");
                    const rapidjson::Value& floorMappingArray = building[FloorMapping.c_str()];
                    
                    std::map<int, std::string> floorMapping;
                    for(rapidjson::SizeType j = 0; j < floorMappingArray.Size(); ++j)
                    {
                        floorMapping[floorMappingArray[j][BuildingFloorIndex.c_str()].GetInt()] = floorMappingArray[j][TrackedFloorIndex.c_str()].GetString();
                    }
                    
                    SdkModel::ApplicationInteriorTrackingInfo interiorTrackingInfo(interiorId,
                                                                                   type,
                                                                                   interiorTrackingConfig,
                                                                                   floorMapping);
                    
                    interiorTrackingInfoList.insert(std::pair<std::string, SdkModel::ApplicationInteriorTrackingInfo>(interiorId.Value(),interiorTrackingInfo));
                }
            }
        }
    }
}
