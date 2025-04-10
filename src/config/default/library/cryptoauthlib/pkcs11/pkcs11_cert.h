/**
 * \file
 * \brief PKCS11 Library Certificate Handling
 *
 * \copyright (c) 2015-2020 Microchip Technology Inc. and its subsidiaries.
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip software
 * and any derivatives exclusively with Microchip products. It is your
 * responsibility to comply with third party license terms applicable to your
 * use of third party software (including open source software) that may
 * accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
 * PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT,
 * SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE
 * OF ANY KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF
 * MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE
 * FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL
 * LIABILITY ON ALL CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED
 * THE AMOUNT OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR
 * THIS SOFTWARE.
 */

#ifndef PKCS11_CERT_H_
#define PKCS11_CERT_H_

#include "pkcs11_object.h"

#ifdef __cplusplus
extern "C" {
#endif

extern const pkcs11_attrib_model pkcs11_cert_x509public_attributes[];
extern const CK_ULONG pkcs11_cert_x509public_attributes_count;

extern const pkcs11_attrib_model pkcs11_cert_wtlspublic_attributes[];
extern const CK_ULONG pkcs11_cert_wtlspublic_attributes_count;

extern const pkcs11_attrib_model pkcs11_cert_x509_attributes[];
extern const CK_ULONG pkcs11_cert_x509_attributes_count;

CK_RV pkcs11_cert_x509_write(CK_VOID_PTR pObject, CK_ATTRIBUTE_PTR pAttribute, pkcs11_session_ctx_ptr pSession);
CK_RV pkcs11_cert_load(pkcs11_object_ptr pObject, CK_ATTRIBUTE_PTR pAttribute, ATCADevice device);
CK_RV pkcs11_cert_clear_session_cache(pkcs11_session_ctx_ptr session_ctx);
CK_RV pkcs11_cert_clear_object_cache(pkcs11_object_ptr pObject);

#ifdef __cplusplus
}
#endif

#endif /* PKCS11_CERT_H_ */
