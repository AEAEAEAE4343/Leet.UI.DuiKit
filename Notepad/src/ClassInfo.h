/*
 *
 * The MIT License (MIT)
 * Copyright (c) 2016 Chuyu-Team
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * Note: This code was not taken directly from MishaProduction's repository.
 * Instead it was taken from ConsoleLogonHook by wiktorwiktor12. Misha
 * contacted me to ask if I could add proper accreditation, so I now add this
 * copyright notice.
 *
 */

#include "stdafx.h"

namespace DirectUI 
{
	template<typename Class, typename BaseClass>
	class ClassInfo : ClassInfoBase
	{
	public:
		HRESULT CreateInstance(Element* rootElement, unsigned long* unknown, Element** result) override
		{
			return Class::CreateInstance(rootElement, unknown, result);
		}
		IClassInfo* GetBaseClass() override
		{
			return BaseClass::GetClassInfoPtr();
		}
		HRESULT DoRegister() { return ClassInfoBase::Register(); }

		void Destroy() override
		{
			delete this;
			Class::Class = NULL;
		}

		static HRESULT Create(ClassInfo** result, HINSTANCE hinst)
		{
			ClassInfo* theClass = new ClassInfo();

			memset(theClass->data, 0, sizeof(theClass->data));

			HRESULT hr = theClass->Initialize(hinst, (UCString)Class::DoGetClassName(), false, NULL, 0);
			if (SUCCEEDED(hr))
			{
				*result = theClass;
			}
			return hr;
		}
		static HRESULT Register(HINSTANCE hinst)
		{
			HRESULT hr = S_OK;

			if (BaseClass::GetClassInfoPtr() == NULL)
			{
				hr = BaseClass::Register();
			}
			else
			{
				BaseClass::GetClassInfoPtr()->AddRef();
			}

			if (SUCCEEDED(hr))
			{
				CritSecLock lock(Element::GetFactoryLock());

				IClassInfo* baseclassptr = BaseClass::GetClassInfoPtr();
				IClassInfo* newrclass = Class::GetClassInfoPtr();

				if (DirectUI::ClassInfoBase::ClassExist(&baseclassptr, NULL, 0, newrclass, hinst, (UCString)Class::DoGetClassName(), false))
				{
					Class::Class = baseclassptr;
				}
				else
				{
					ClassInfo* classInfo = NULL;
					if (SUCCEEDED(hr = Create(&classInfo, hinst)))
					{
						if (SUCCEEDED(hr = classInfo->DoRegister()))
						{
							Class::Class = (IClassInfo*)classInfo;
						}
						else
						{
							classInfo->Destroy();
						}
					}
				}
			}
			return hr;
		}
	};
}