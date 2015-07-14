// Copyright (c) Microsoft Corporation.  All rights reserved

#pragma once

namespace svgpp
{
    using namespace Windows::Data::Xml::Dom;

    class AttributeEnumerator
    {
    public:
        AttributeEnumerator(IXmlNode^ node)
        {            
            throw E_UNEXPECTED;
        }

        bool IsEnd() const
        {
            throw E_UNEXPECTED;
        }

        void Advance()
        {
            throw E_UNEXPECTED;
        }

        detail::namespace_id GetNamespace() const
        {
            throw E_UNEXPECTED;
        }

        Platform::String^ GetLocalName() const
        {
            throw E_UNEXPECTED;
        }

        Platform::String^ GetValue() const
        {
            throw E_UNEXPECTED;
        }

        IXmlNode^ SaveValue() const
        {
            throw E_UNEXPECTED;
        }
    };

    namespace policy
    {
        namespace xml
        {
            template<>
            struct attribute_iterator<AttributeEnumerator>
            {
                typedef boost::iterator_range<wchar_t const*> string_type;
                typedef Platform::String^ attribute_name_type;
                typedef Platform::String^ attribute_value_type;
                typedef IXmlNode^ saved_value_type;

                static bool is_end(AttributeEnumerator const& enumerator)
                {
                    return enumerator.IsEnd();
                }

                static void advance(AttributeEnumerator& enumerator)
                {
                    enumerator.Advance();
                }

                static Platform::String^ get_local_name(AttributeEnumerator const& enumerator)
                {
                    return enumerator.GetLocalName();
                }

                static detail::namespace_id get_namespace(AttributeEnumerator const& enumerator)
                {
                    return enumerator.GetNamespace();
                }

                static string_type get_string_range(Platform::String^ str)
                {
                    auto data = str->Data();

                    return string_type(data, data + str->Length());
                }

                static AttributeEnumerator get_attribute(AttributeEnumerator enumerator)
                {
                    return enumerator;
                }
                
                static Platform::String^ get_value(AttributeEnumerator const& enumerator)
                {
                    return enumerator.GetValue();
                }

                static IXmlNode^ save_value(AttributeEnumerator const& enumerator)
                {
                    return enumerator.SaveValue();
                }
            };

            template<>
            struct element_iterator<IXmlNode^>
            {
                typedef boost::iterator_range<wchar_t const*> string_type;
                typedef IXmlNode^ iterator_type;
                typedef AttributeEnumerator attribute_enumerator_type;

                typedef Platform::String^ element_name_type;

                static element_name_type get_local_name(iterator_type xml_element)
                {
                    return safe_cast<Platform::String^>(xml_element->LocalName);
                }

                static string_type get_string_range(Platform::String^ str)
                {
                    auto data = str->Data();

                    return string_type(data, data + str->Length());
                }

                static attribute_enumerator_type get_attributes(IXmlNode^ const xml_node)
                {
                    return AttributeEnumerator(xml_node);
                }

                static iterator_type get_child_elements(IXmlNode^ const xml_node)
                {
                    throw E_UNEXPECTED;
                }

                static bool is_end(IXmlNode^ const xml_node)
                {
                    throw E_UNEXPECTED;
                }

                static void advance_element(IXmlNode^ xml_node)
                {
                    throw E_UNEXPECTED;
                }
            };
        }
    }
}
