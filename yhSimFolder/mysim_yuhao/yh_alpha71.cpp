#include "include/qlSim.h"
namespace
{
class alpha71: public AlphaBase
{
    public:
    explicit alpha71(XMLCONFIG::Element *cfg):
        AlphaBase(cfg), 
        vwap(dr.getData<QL_MATRIX<QL_FLOAT>>("adj_vwap")), 
        vol(dr.getData<QL_MATRIX<QL_FLOAT>>("volume")),  
        low(dr.getData<QL_MATRIX<QL_FLOAT>>("adj_low")),
        open(dr.getData<QL_MATRIX<QL_FLOAT>>("adj_open")),
        close(dr.getData<QL_MATRIX<QL_FLOAT>>("adj_close")),
        Num1(cfg->getAttributeIntDefault("para1",3)),
	Num2(cfg->getAttributeIntDefault("para2",180)),
	Num3(cfg->getAttributeIntDefault("para3",12)),
	Num4(cfg->getAttributeIntDefault("para4",18)),
	Num5(cfg->getAttributeIntDefault("para5",4)),
	Num6(cfg->getAttributeIntDefault("para6",15)),
	Num7(cfg->getAttributeIntDefault("para7",16))
	
    {
    	adv180.resize (GLOBAL::Dates.size());

    	for (int di = 0; di< GLOBAL::Dates.size(); ++di)
    	{
    		adv180[di].resize(GLOBAL::Instruments.size(),nan);
    	}

    	cor_de.resize (GLOBAL::Dates.size());

    	for (int di = 0; di< GLOBAL::Dates.size(); ++di)
    	{
    		cor_de[di].resize(GLOBAL::Instruments.size(),nan);
    	}

    	cor_de2.resize (GLOBAL::Dates.size());

    	for (int di = 0; di< GLOBAL::Dates.size(); ++di)
    	{
    		cor_de2[di].resize(GLOBAL::Instruments.size(),nan);
    	}

    	lov2.resize (GLOBAL::Dates.size());

    	for (int di = 0; di< GLOBAL::Dates.size(); ++di)
    	{
    		lov2[di].resize(GLOBAL::Instruments.size(),nan);
    	}

 		dec.resize (GLOBAL::Dates.size());

    	for (int di = 0; di< GLOBAL::Dates.size(); ++di)
    	{
    		dec[di].resize(GLOBAL::Instruments.size(),nan);
    	}
    }
    void generate(int di) override
    {


       for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {
                vector<float> c1(Num2,nan);
                for (int j = 0; j < Num2; ++ j)
                {
                    c1[j] = vol[di - delay - j][ii];
                }
                adv180[di-delay][ii] = QL_Oputils::mean(c1);
            }
        }

       for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {
            	vector<float> c1(Num1,nan);            	
            	vector<float> c4(Num4,nan);
            	vector<float> c3(Num3,nan);
            	vector<float> c5(Num4,nan);

		for (int k =0; k<Num4;++k){

		for (int i =0; i< Num1; ++i){
			c1[i] = close[di-delay-i-k][ii];
		}
		QL_Oputils::rank(c1);
		c4[k] = c1[0];
		
		for (int j =0; j<Num3; ++j){
			c3[j] = adv180[di-delay-j-k][ii];
		}
		QL_Oputils::rank(c3);
		c5[k] = c3[0];
		}
		  cor_de[di-delay][ii] = QL_Oputils::corr(c4,c5);
            }
        }
        
        for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {
                    cor_de2[di-delay][ii] = 0.0;
            	for (int m =0; m<Num5; ++m){
            		cor_de2[di-delay][ii] += float(cor_de[di-delay-m][ii])*(Num5-m) / ((Num5+1)*Num5/2.0);
            	}
            }
    	}

        for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {
                lov2[di-delay][ii] = low[di-delay][ii]+open[di-delay][ii]-2* vwap[di-delay][ii];
            }
        }

        for(int i = di-delay - Num5 - Num7; i < di; ++ i)
        {

                QL_Oputils::rank(lov2[di-delay-i]);
        
        }


        for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {
                dec[di-delay][ii] = 0.0;
            	for (int m =0; m<Num7; ++m){

            		dec[di-delay][ii] += float(lov2[di-delay-m][ii])*float(lov2[di-delay-m][ii])*(Num7-m) / ((Num7+1)*Num7/2.0);
            	}
            }
    	}
	
        for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
	     
            {
            	vector<float> c7(Num6,nan);
            	vector<float> c8(Num5,nan);

		for (int m=0;m<Num6;++m){
			c7[m]= cor_de2[di-delay-m][ii];
		}
		QL_Oputils::rank(c7);
		

		for (int b =0;b<Num5;++b){

			c8[b] = dec[di-delay-b][ii];
		}

		QL_Oputils::rank(c8);

		if (float(c7[0])<float(c8[0]))
            {alpha[ii]=float(c8[0]);}
			else {alpha[ii]=float(c7[0]);}
		

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
        }
	const QL_MATRIX<QL_FLOAT> &vwap;
	const QL_MATRIX<QL_FLOAT> &vol;
	const QL_MATRIX<QL_FLOAT> &low;
	const QL_MATRIX<QL_FLOAT> &open;
	const QL_MATRIX<QL_FLOAT> &close;
	vector<vector<float> > adv180;
	vector<vector<float> > cor_de;
	vector<vector<float> > cor_de2;
	vector<vector<float> > lov2;
	vector<vector<float> > dec;
        int Num1; 
	int Num2;
	int Num3;
	int Num4;
	int Num5;
	int Num6;
	int Num7;
};
}
extern "C"
{
    AlphaBase * createStrategy(XMLCONFIG::Element *cfg)
    {
        AlphaBase * str = new alpha71(cfg);
        return str;
    }
}

