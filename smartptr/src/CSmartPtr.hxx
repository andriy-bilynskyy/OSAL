#ifndef CSMARTPTR_H_
#define CSMARTPTR_H_

#include <CSmartPtrCfg.hxx>

template <typename T>
class CSmartPtr
{
public:

    CSmartPtr();
    CSmartPtr(T * pValue);
    ~CSmartPtr();
    CSmartPtr(const CSmartPtr<T> & sptr);
    T & operator * ();
    T* operator -> ();
    CSmartPtr<T> & operator = (const CSmartPtr<T> & sptr);

private:

    T              * m_pData;
    int            * m_pRefCnt;
    lockObjectType   m_protector;
};

template <typename T>
CSmartPtr<T>::CSmartPtr()
{
    lockObjectINIT(m_protector);
    lockObjectLOCK(m_protector);
    m_pData    = 0;
    m_pRefCnt  = new int;
    *m_pRefCnt = 1;
    lockObjectUNLOCK(m_protector);
}

template <typename T>
CSmartPtr<T>::CSmartPtr(T * pValue)
{
    lockObjectINIT(m_protector);
    lockObjectLOCK(m_protector);
    m_pData    = pValue;
    m_pRefCnt  = new int;
    *m_pRefCnt = 1;
    lockObjectUNLOCK(m_protector);
}

template <typename T>
CSmartPtr<T>::~CSmartPtr()
{
    lockObjectLOCK(m_protector);
    if(!(--(*m_pRefCnt)))
    {
        if(m_pData)
        {
            delete m_pData;
        }
        delete m_pRefCnt;
    }
    lockObjectUNLOCK(m_protector);
    lockObjectDESTROY(m_protector);
}

template <typename T>
CSmartPtr<T>::CSmartPtr(const CSmartPtr<T> & sptr)
{
    lockObjectLOCK(m_protector);
    m_pData   = sptr.m_pData;
    m_pRefCnt = sptr.m_pRefCnt;
    (*m_pRefCnt)++;
    lockObjectUNLOCK(m_protector);
}

template <typename T>
T & CSmartPtr<T>::operator * ()
{
    lockObjectLOCK(m_protector);
    T & result= *m_pData;
    lockObjectUNLOCK(m_protector);
    return result;
}

template <typename T>
T* CSmartPtr<T>::operator -> ()
{
    lockObjectLOCK(m_protector);
    T * result = m_pData;
    lockObjectUNLOCK(m_protector);
    return result;
}

template <typename T>
CSmartPtr<T> & CSmartPtr<T>::operator = (const CSmartPtr<T> & sptr)
{
    lockObjectLOCK(m_protector);
    if (this != &sptr)
    {
        if(!(--(*m_pRefCnt)))
        {
            if(m_pData)
            {
                delete m_pData;
            }
            delete m_pRefCnt;
        }
        m_pData   = sptr.m_pData;
        m_pRefCnt = sptr.m_pRefCnt;
        (*m_pRefCnt)++;
    }
    lockObjectUNLOCK(m_protector);
    return *this;
}

#endif
