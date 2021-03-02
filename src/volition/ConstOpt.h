// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_CONSTOPT_H
#define VOLITION_CONSTOPT_H

namespace Volition {

//================================================================//
// ConstOpt
//================================================================//
template < typename TYPE >
class ConstOpt {
private:

    const TYPE*     mConst;
    TYPE*           mMutable;

public:

    //----------------------------------------------------------------//
    operator bool () const {
        return ( this->mConst != NULL );
    }

    //----------------------------------------------------------------//
    operator TYPE* () {
        assert ( this->mMutable );
        return this->mMutable;
    }

    //----------------------------------------------------------------//
    operator const TYPE* () const {
        assert ( this->mConst );
        return this->mConst;
    }

    //----------------------------------------------------------------//
    operator TYPE& () {
        return this->get ();
    }

    //----------------------------------------------------------------//
    operator const TYPE& () const {
        return this->get ();
    }

    //----------------------------------------------------------------//
    TYPE& operator * () {
        return this->get ();
    }

    //----------------------------------------------------------------//
    const TYPE& operator * () const {
        return this->get ();
    }

    //----------------------------------------------------------------//
    TYPE* operator -> () {
        assert ( this->mMutable );
        return this->mMutable;
    }

    //----------------------------------------------------------------//
    const TYPE* operator -> () const {
        assert ( this->mConst );
        return this->mConst;
    }

    //----------------------------------------------------------------//
    ConstOpt () :
        mConst ( NULL ),
        mMutable ( NULL ) {
    }

    //----------------------------------------------------------------//
    ConstOpt ( const TYPE& value ) :
        mConst ( &value ),
        mMutable ( NULL ) {
    }
    
    //----------------------------------------------------------------//
    ConstOpt ( TYPE& value ) :
        mConst ( &value ),
        mMutable ( &value ) {
    }
    
    //----------------------------------------------------------------//
    const TYPE& get () const {
        return this->getConst ();
    }
    
    //----------------------------------------------------------------//
    TYPE& get () {
        return this->getMutable ();
    }
    
    //----------------------------------------------------------------//
    const TYPE& getConst () const {
        assert ( this->mConst );
        return *this->mConst;
    }
    
    //----------------------------------------------------------------//
    TYPE& getMutable () {
        assert ( this->mMutable );
        return *this->mMutable;
    }
    
    //----------------------------------------------------------------//
    bool isConst () {
        return ( this->mMutable == NULL );
    }

    //----------------------------------------------------------------//
    bool isMutable () {
        return ( this->mMutable != NULL );
    }
    
    //----------------------------------------------------------------//
    void set ( const TYPE& value ) {
        this->mConst = &value;
        this->mMutable = NULL;
    }
    
    //----------------------------------------------------------------//
    void set ( TYPE& value ) {
        this->mConst = &value;
        this->mMutable = &value;
    }
};

} // namespace Volition
#endif
