#pragma once

#include "Base.h"

struct CTexture {
	LPDIRECT3DTEXTURE9 Handle;

	CTexture() : Handle(nullptr) {}

	CTexture(
		LPDIRECT3DTEXTURE9 Handle
	) : Handle(Handle) {}

	CTexture(
		const CTexture& Other
	) : Handle(Other.Handle) {
		if (Handle) {
			Handle->AddRef();
		}
	}

	CTexture(
		CTexture&& Other
	) noexcept
		: Handle(Other.Handle) {
		Other.Handle = nullptr;
	}

	~CTexture() {
		if (Handle) {
			Handle->Release();
		}
	}

	CTexture& operator=(const CTexture& Other) {
		if (this != &Other) {
			if (Handle) {
				Handle->Release();
			}

			Handle = Other.Handle;
			if (Handle) {
				Handle->AddRef();
			}
		}

		return *this;
	}

	CTexture& operator=(CTexture&& Other) noexcept {
		if (this != &Other) {
			if (Handle) {
				Handle->Release();
			}

			Handle = Other.Handle;
			Other.Handle = nullptr;
		}

		return *this;
	}
};
