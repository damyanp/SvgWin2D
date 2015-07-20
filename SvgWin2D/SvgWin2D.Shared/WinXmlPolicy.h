// Copyright (c) Microsoft Corporation.  All rights reserved

#pragma once

namespace svgpp
{
    using namespace Windows::Data::Xml::Dom;
    using namespace Windows::Foundation::Collections;
    
    //
    // This is required because SVG++ was creating arrays of IXmlNode^'s and
    // these were being initialized to 0xCCCCCCCC rather than 0, resulting in an
    // AV when attempting to assign the new value.  Wrapping in the SavedValue
    // value type fixes this.
    //
    class SavedValue
    {
        IXmlNode^ m_node;

    public:
        SavedValue()
        {}

        SavedValue(IXmlNode^ node)
            : m_node(node)
        {}

        Platform::String^ GetValue()
        {
            return dynamic_cast<Platform::String^>(m_node->NodeValue);
        }
    };

    class AttributeEnumerator
    {
        IIterator<IXmlNode^>^ m_iterator;

    public:
        AttributeEnumerator(IXmlNode^ node)
        {            
            auto attributes = node->Attributes;
            m_iterator = attributes->First();
        }

        bool IsEnd() const
        {
            return !m_iterator->HasCurrent;
        }

        void Advance()
        {
            m_iterator->MoveNext();
        }

        detail::namespace_id GetNamespace() const
        {
            auto namespaceString = dynamic_cast<Platform::String^>(m_iterator->Current->NamespaceUri);

            if (namespaceString == nullptr)
                return detail::namespace_id::svg;

            boost::iterator_range<wchar_t const *> ns_uri = boost::as_literal(namespaceString->Data());

            if (boost::range::equal(detail::xml_namespace_uri<wchar_t>(), ns_uri))
                return detail::namespace_id::xml;
            else if (boost::range::equal(detail::xlink_namespace_uri<wchar_t>(), ns_uri))
                return detail::namespace_id::xlink;
            return detail::namespace_id::other;            
        }

        Platform::String^ GetLocalName() const
        {
            return safe_cast<Platform::String^>(m_iterator->Current->LocalName);
        }

        Platform::String^ GetValue() const
        {
            return safe_cast<Platform::String^>(m_iterator->Current->NodeValue);
        }

        IXmlNode^ SaveValue() const
        {
            return m_iterator->Current;
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
                typedef SavedValue saved_value_type;

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

                static SavedValue save_value(AttributeEnumerator const& enumerator)
                {
                    return SavedValue(enumerator.SaveValue());
                }

                static Platform::String^ get_value(SavedValue value)
                {
                    return value.GetValue();
                }
            };

            template<>
            struct element_iterator<IXmlNode^>
            {
                typedef boost::iterator_range<wchar_t const*> string_type;
                typedef IXmlNode^ iterator_type;
                typedef AttributeEnumerator attribute_enumerator_type;

                typedef Platform::String^ element_name_type;

                static element_name_type get_local_name(IXmlNode^ const node)
                {
                    auto type = node->GetType()->FullName;
                    auto nodeType = node->NodeType.ToString();

                    auto name = safe_cast<Platform::String^>(node->LocalName);
                    
                    return name;
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
                    auto node = xml_node->FirstChild;
                    while (node != nullptr && node->NodeType != NodeType::ElementNode)
                    {
                        node = node->NextSibling;
                    }
                    return node;
                }

                static IXmlNode^ FindNextElement(IXmlNode^ node)
                {
                    do
                    {
                        node = node->NextSibling;
                    } while (node != nullptr && node->NodeType != NodeType::ElementNode);

                    return node;
                }

                static bool is_end(IXmlNode^ const node)
                {
                    return node == nullptr;
                }

                static void advance_element(IXmlNode^& node)
                {
                    node = FindNextElement(node);
                }
            };
        }
    }
}
