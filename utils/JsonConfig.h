#pragma once

//////////////////////////////////////////////////////////////////////////
#include <json/json.h>
#include <string>
#include <vector>
#include <boost/exception/all.hpp>

class CJsonConfigDialog;

//////////////////////////////////////////////////////////////////////////
class CJsonConfig
{
public:
    CJsonConfig( const std::string& congName );
    virtual ~CJsonConfig();

protected:
    void Flush();

    template<class _JType>
    void SetValue( const std::string& path, const _JType& value )
    {
        Json::Value* jpVal = FindJsonValue( path );
        (*jpVal) = value;
        Flush();
    }

    template<class _JType>
    _JType GetValue( const std::string& path ) const
    {
        const Json::Value* jpVal = const_cast<const Json::Value*>(FindJsonValue( path ));
        return jpVal->as<_JType>();
    }

    Json::Value* FindJsonValue( const std::string& path );
    const Json::Value* FindJsonValue( const std::string& path ) const;

private:
    void Init();
    static std::vector<std::string> SplitPath( const std::string& path );
    static std::vector<std::string> SplitPathWithCheckEmpty( const std::string& path );

private:
    std::string m_fileName;
    Json::Value m_storage;
};

