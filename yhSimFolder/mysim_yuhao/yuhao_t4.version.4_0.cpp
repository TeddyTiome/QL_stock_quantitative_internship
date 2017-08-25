#include "include/qlSim.h"
namespace
{
class t4: public AlphaBase
{
    public:
    explicit t4(XMLCONFIG::Element *cfg):
        AlphaBase(cfg), 
        close(dr.getData<QL_MATRIX<QL_FLOAT>>("adj_close")),
        high(dr.getData<QL_MATRIX<QL_FLOAT>>("adj_high")),
        low(dr.getData<QL_MATRIX<QL_FLOAT>>("adj_low")),
        Num1(cfg->getAttributeIntDefault("para1",5)),
	    Num2(cfg->getAttributeIntDefault("para2",20)),
        Num3(cfg->getAttributeIntDefault("para3",10))

    {

          

        atr.resize(GLOBAL::Dates.size());
        for (int di =0; di<GLOBAL::Dates.size(); ++di)
        {
            atr[di].resize(GLOBAL::Instruments.size(),nan);
        }        

        ma.resize(GLOBAL::Dates.size());
        for (int di =0; di<GLOBAL::Dates.size(); ++di)
        {
            ma[di].resize(GLOBAL::Instruments.size(),nan);
        }     

        atr_ts.resize(GLOBAL::Dates.size());
        for (int di =0; di<GLOBAL::Dates.size(); ++di)
        {
            atr_ts[di].resize(GLOBAL::Instruments.size(),nan);
        }        

        tr_ts.resize(GLOBAL::Dates.size());
        for (int di =0; di<GLOBAL::Dates.size(); ++di)
        {
            tr_ts[di].resize(GLOBAL::Instruments.size(),nan);
        }      
     
        tr.resize(GLOBAL::Dates.size());
        for (int di =0; di<GLOBAL::Dates.size(); ++di)
        {
            tr[di].resize(GLOBAL::Instruments.size(),nan);
        }   


    }
    void generate(int di) override
    {

         for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {

                vector<float> c1(3,nan);

                c1[0] = high[di-delay][ii] - low[di-delay][ii];
                c1[1] = fabs(close[di-delay - 1][ii] - high[di-delay][ii]);
                c1[2] = fabs(low[di-delay][ii] - close[di-delay-1][ii]);

                tr[di-delay][ii] = QL_Oputils::hhv(c1.begin(),c1.end()) ;
            }
        }

       for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {

                vector<float> c1(Num1,nan);

                for (int j = 0; j < Num1; ++ j)
                {
                    c1[j] = tr[di - delay - j][ii];
                }  

                atr[di-delay][ii] = QL_Oputils::mean(c1);                

                vector<float> c2(Num2,nan);

                for (int i = 0; i < Num2; ++ i)
                {
                    c2[i] = close[di - delay - i][ii];
                }  

                ma[di-delay][ii] = QL_Oputils::mean(c2);
            }
        }

       for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {

                vector<float> c1(Num1,nan);

                for (int j = 0; j < Num1; ++ j)
                {
                    c1[j] = atr[di - delay - j][ii];
                }  
                QL_Oputils::rank(c1);

                atr_ts[di-delay][ii] = c1[0];                

                vector<float> c2(Num2,nan);

                for (int i = 0; i < Num2; ++ i)
                {
                    c2[i] = tr[di - delay - i][ii];
                }  
                QL_Oputils::rank(c2);

                tr_ts[di-delay][ii] = c2[0];
            }
        }
  

        // QL_Oputils::rank(tr[di-delay]);
        // // QL_Oputils::rank(ma[di-delay]);
        // QL_Oputils::rank(atr[di-delay]);

        for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {
                if (!(GLOBALFUNC::isnan(atr[di-delay][ii]))&&!(GLOBALFUNC::equal(atr[di-delay][ii],0))&&!(GLOBALFUNC::isnan(atr[di-delay-1][ii]))&&!(GLOBALFUNC::equal(atr[di-delay-1][ii],0)))
                {
                if (close[di-delay][ii]>(ma[di-delay][ii] + 0.5*atr[di-delay][ii]))
                {
                    alpha[ii] =  -1*((tr[di-delay][ii] - ma[di-delay][ii]) / atr[di-delay][ii] - (tr[di-delay-1][ii] - ma[di-delay-1][ii]) / atr[di-delay-1][ii]);
                }
                else if (close[di-delay][ii]<(ma[di-delay][ii] - 0.5*atr[di-delay][ii]))
                {
                    alpha[ii] =  ((tr[di-delay][ii] - ma[di-delay][ii] ) / atr[di-delay][ii] - (tr[di-delay-1][ii] - ma[di-delay-1][ii] ) / atr[di-delay-1][ii]);                     
                }
                else {alpha[ii] = 0;}
                }
             else {alpha[ii] = 0;}

	       }
        }


        return;


		}
		
    
    
    
    void checkPointSave(boost::archive::binary_oarchive &ar) 
    {
        ar & *this;
    }
    void checkPointLoad(boost::archive::binary_iarchive &ar)
    {
        ar & *this;
    }
    std::string version() const
    {
        return GLOBALCONST::VERSION;
    }
    private:
        friend class boost::serialization::access;
        template<typename Archive>
        void serialize(Archive & ar, const unsigned int/*file_version*/)
        {
           
            ar & atr;
            ar & ma;
            ar & atr_ts;
            ar & tr_ts;
            ar & tr;
        }
    const QL_MATRIX<QL_FLOAT> &close;
    const QL_MATRIX<QL_FLOAT> &high;
    const QL_MATRIX<QL_FLOAT> &low;

    vector<vector<float> > atr;
    vector<vector<float> > ma;
    vector<vector<float> > atr_ts;
    vector<vector<float> > tr_ts;
    vector<vector<float> > tr;

    int Num1; 
	int Num2;
    int Num3;
};
}
extern "C"
{
    AlphaBase * createStrategy(XMLCONFIG::Element *cfg)
    {
        AlphaBase * str = new t4(cfg);
        return str;
    }
}

