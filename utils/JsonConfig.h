#pragma once

//////////////////////////////////////////////////////////////////////////
#include <string>
#include <vector>
#include <stack>
#include <mutex>
#include <boost/describe.hpp>
#include <boost/json.hpp>
#include <boost/mp11.hpp>
#include <type_traits>

//////////////////////////////////////////////////////////////////////////
BOOST_DEFINE_ENUM_CLASS( ETypeValue,
    NullValue,
    BooleanValue,
    IntegerValue,
    UnsignedValue,
    FloatValue,
    StringValue,
    HexNumberString,    /** number as hex string */
    ArrajValue,         /** json format array (check with json parser) */
    ObjectValue,        /** json format object (check with json parser) */
    LinkValue           /** full path to linked object */
)

class CJsonConfigDialog;
namespace Json
{
    class Value;
}

template<class T> void extract( boost::json::object const & obj, char const * name, T & value )
{
    value = boost::json::value_to<T>( obj.at( name ) );
}

template<class T,
    class D1 = boost::describe::describe_members<T,
        boost::describe::mod_public | boost::describe::mod_protected>,
    class D2 = boost::describe::describe_members<T, boost::describe::mod_private>,
    class En = std::enable_if_t<boost::mp11::mp_empty<D2>::value && !std::is_union<T>::value> >

    T tag_invoke( boost::json::value_to_tag<T> const&, boost::json::value const& v )
{
    auto const& obj = v.as_object();

    T t{};

    boost::mp11::mp_for_each<D1>([&](auto D){

        extract( obj, D.name, t.*D.pointer );

    });

    return t;
}

template<class T,
    class D1 = boost::describe::describe_members<T,
        boost::describe::mod_public | boost::describe::mod_protected>,
    class D2 = boost::describe::describe_members<T, boost::describe::mod_private>,
    class En = std::enable_if_t<boost::mp11::mp_empty<D2>::value && !std::is_union<T>::value> >

    void tag_invoke( boost::json::value_from_tag const&, boost::json::value& v, T const & t )
{
    auto& obj = v.emplace_object();

    boost::mp11::mp_for_each<D1>([&](auto D){

        obj[ D.name ] = boost::json::value_from( t.*D.pointer );

    });
}

//////////////////////////////////////////////////////////////////////////
class CJsonConfig
{
public:
    CJsonConfig( const std::string_view congName );
    CJsonConfig( const CJsonConfig&) = delete;
    virtual ~CJsonConfig();

    bool IsUndoAvailable() const;
    void SaveCurrentPropertySet();
    void Undo();
    void ClearUndo();

    static bool ReadFromStream(std::istream& is, Json::Value& jValue, std::string& errs);
    static void WriteToStream( std::ostream& os, const Json::Value& jValue );
    static bool IsTextJsonArray( const std::string& text );
    static bool IsTextJsonObject( const std::string& text );

protected:
    void Flush();

    template<class _JType>
    void SetValue( std::string_view path, const _JType& value );

    template<class _JType>
    top::optional<_JType> GetValue( std::string_view path ) const;

    Json::Value& FindJsonValue( std::string_view path );
    top::optional<const Json::Value&> FindJsonValue( std::string_view path ) const;
    
    template<class _PropsSet>
    static _PropsSet FromJsonString( std::string_view jStr );

    template<class _PropsSet>
    static std::string ToJsonString( const _PropsSet& props );

    static bool IsNull( const Json::Value& jv ) ;
    static bool Empty( const Json::Value& jv );
    static bool IsArray( const Json::Value& jv );
    static bool IsObject( const Json::Value& jv );
    static std::vector<std::string> GetMemberNames( const Json::Value& jv );
    static size_t Size( const Json::Value& jv );
    static const Json::Value& At( const Json::Value& jv, unsigned idx );

    static uint64_t ToUInt( const Json::Value& jv );
    static int64_t ToInt( const Json::Value& jv );
    static float ToFloat( const Json::Value& jv );
    static double ToDouble( const Json::Value& jv );
    static std::string ToString( const Json::Value& jv );
    static bool ToBool( const Json::Value& jv );

    static void Assign( Json::Value& jv, uint64_t val );
    static void Assign( Json::Value& jv, int64_t val );
    static void Assign( Json::Value& jv, float val );
    static void Assign( Json::Value& jv, double val );
    static void Assign( Json::Value& jv, const std::string& val );
    static void Assign( Json::Value& jv, bool val );
    static void Assign( Json::Value& jv, const Json::Value& val );

private:
    void Init();
    Json::Value& Storage();
    const Json::Value& Storage() const;
    Json::Value& ValueReference( Json::Value& parent, const std::vector<std::string>& p, size_t idx );
    top::optional<const Json::Value&> ValueReference( const Json::Value& parent, const std::vector<std::string>& p, size_t idx ) const;
    static std::vector<std::string> SplitPath( std::string_view path );
    static std::vector<std::string> SplitPathWithCheckEmpty( std::string_view path );

private:
    mutable std::recursive_mutex m_mutex;
    std::string m_fileName;
    std::unique_ptr<Json::Value> m_storage;
    std::stack<std::unique_ptr<Json::Value>> m_undo;
};

template<class _PropsSet>
inline std::string CJsonConfig::ToJsonString( const _PropsSet& props )
{
    std::ostringstream oss;
    oss << boost::json::value_from( props );
    return oss.str();
}


template<class _PropsSet>
inline _PropsSet CJsonConfig::FromJsonString( std::string_view jStr )
{
    auto parse = [](const std::string& js) -> boost::json::value
    {
        std::istringstream f( js );
        boost::json::stream_parser p;
        boost::json::error_code ec;
        do
        {
            std::string line;
            std::getline(f, line);
            p.write(line.c_str(), line.size(), ec);
        }
        while( ! f.eof() );
        if( ec )
            return nullptr;
        p.finish( ec );
        if( ec )
            return nullptr;
        return p.release();
    };

    boost::json::value jv(parse(jStr));
    _PropsSet props = boost::json::value_to<_PropsSet>( jv );
    return props;
}

template<class _JType>
inline top::optional<_JType> CJsonConfig::GetValue( std::string_view path ) const
{
    std::lock_guard<std::recursive_mutex> lock(m_mutex);
    top::optional<const Json::Value&> jpValOp = FindJsonValue(path);
    if (jpValOp.has_value())
    {
        if constexpr (std::is_signed_v<_JType>)
        {
            return (_JType)ToInt( jpValOp.get() );
        }
        else if constexpr (std::is_unsigned_v<_JType>)
        {
            return (_JType)ToUInt( jpValOp.get() );
        }
        else if constexpr (std::is_same_v<_JType, float>)
        {
            return (float)ToFloat( std::is_same_v<_JType, float> );
        }
        else if constexpr (std::is_same_v<_JType, double>)
        {
            return ToDouble( jpValOp.get() );
        }
        else if constexpr (std::is_same_v<_JType, bool>)
        {
            return ToBool( jpValOp.get() );
        }
        else
        {
            return ToString( jpValOp.get() );
        }
    }
    return {};
}

template<class _JType>
inline void CJsonConfig::SetValue( std::string_view path, const _JType& value )
{
    std::lock_guard<std::recursive_mutex> lock(m_mutex);
    Json::Value& jpVal = FindJsonValue( path );
    if constexpr (std::is_signed_v<_JType>)
    {
        Assign( jpVal, static_cast<int64_t>(value) );
    }
    else if constexpr (std::is_unsigned_v<_JType>)
    {
        Assign( jpVal, static_cast<uint64_t>(value) );
    }
    else
    {
        Assign( jpVal, value );
    }
    Flush();
}

